# $Id: joystick.py 272 2009-03-17 08:59:50Z piman $

# Generic joystick device access, via a Pyglet EventDispatcher.
# Uses Linux native interfaces if available, or SDL if not.

import select
import struct
import glob
import sys

from pyglet.event import EventDispatcher

__all__ = ["Joystick", "JoystickHandler"]

class Joystick(object):
    def __init__(self, number, name, axes, buttons):
        self.number = number

        # Name of the device.
        self.name = name

        # Current axis values, normalizd [-1, 1].
        self.axis = [0.0] * axes

        # Current button state, True/False for each button.
        self.button = [False] * buttons

        # Number of axes.
        self.axes = axes

        # Number of buttons.
        self.buttons = buttons

    def __repr__(self):
        return ("%s(number=%r, name=%r, axes=%r, buttons=%r, "
                "axis=%r, button=%r)") % (
            type(self).__name__, self.number, self.name,
            self.axes, self.buttons, self.axis, self.button)

class BaseJoystickHandler(EventDispatcher):
    # Base joystick handler type. To make it work usefully, implement
    # _scan(self) and dispatch_events(self), then add a case to the
    # 'Joystick = ...' assignment at the end of the file.

    # Based on code from Simon Wittber:
    # http://entitycrisis.blogspot.com/2008/02/pyglet-joystick.html
    # but added saner cross-platform design, ioctl calls for joystick
    # info, current state variables, etc.

    state = []
    joysticks = 0

    def __init__(self):
        """Initialize a joystick."""
        super(EventDispatcher, self).__init__()
        self.scan()

    def scan(self):
        self.state = []
        self._scan()
        self.joysticks = len(self.state)

    def _scan(self):
        pass

    def on_joystick_axis(self, joystick, axis, value):
        """Axis motion.

        The value is normalized on [-1, 1].
        """
        pass

    def on_joystick_button(self, joystick, button, pressed):
        """Button press or release."""
        pass

    def dispatch_events(self):
        pass

BaseJoystickHandler.register_event_type('on_joystick_axis')
BaseJoystickHandler.register_event_type('on_joystick_button')

class LinuxJoystickHandler(BaseJoystickHandler):
    # Poll the joystick file descriptors on Linux.
    # See joystick-api.txt included with the Linux kernel for more.

    # Event structure format --
    __JS_EVENT_FMT = "IhBB"
    __JS_EVENT_SIZE = struct.calcsize(__JS_EVENT_FMT)

    # Event types --
    __JS_EVENT_BUTTON = 0x01 # button pressed/released
    __JS_EVENT_AXIS   = 0x02 # joystick moved
    __JS_EVENT_INIT   = 0x80 # "virtual" event flag

    # ioctl magic codes for device info --
    __JSIOCGAXES    = 0x80016a11    # axis count
    __JSIOCGBUTTONS = 0x80016a12    # button count
    __JSIOCGNAME    = 0x81006a13    # device name

    __devs = []

    def _scan(self):
        # Close the device if it's open.
        try:
            for dev in self.__devs:
                dev.close()
        finally:
            self.__devs = []

        # Try both /dev/input/jsX and the older /dev/jsX.
        for i, filename in enumerate(
            sorted(glob.glob("/dev/input/js*") + glob.glob("/dev/js*"))):
            try:
                dev = open(filename, "rb")
            except (IOError, OSError):
                try:
                    self.__dev = open("/dev/js%d" % self.number, "rb")
                except Exception, err:
                    raise IOError("Error opening joystick %d: %s" % (
                        self.number, err))
            except Exception, err:
                raise IOError("Error opening joystick %d: %s" % (
                    self.number, err))
            else:

                # Grab ioctl data from the file handle right away
                from fcntl import ioctl
                axes = struct.unpack(
                    "b", ioctl(dev, self.__JSIOCGAXES, " "))[0]
                buttons = struct.unpack(
                    "b", ioctl(dev, self.__JSIOCGBUTTONS, " "))[0]
                name = struct.unpack(
                    "1024s", ioctl(dev, self.__JSIOCGNAME, " " * 1024))[0]
                name = name.rstrip().strip("\x00")
                self.__devs.append(dev)
                self.state.append(Joystick(i, name, axes, buttons))

    def dispatch_events(self):
        for joystick, dev in zip(self.state, self.__devs):
            while select.select([dev], [], [], 0)[0]:
                # Unpack the event and send it!
                evt = dev.read(self.__JS_EVENT_SIZE)
                time, value, etype, number = struct.unpack(
                    self.__JS_EVENT_FMT, evt)
                if etype & self.__JS_EVENT_AXIS:
                    joystick.axis[number] = value / 32768.0
                    self.dispatch_event(
                        'on_joystick_axis', joystick, number,
                        joystick.axis[number])
                elif etype & self.__JS_EVENT_BUTTON:
                    joystick.button[number] = bool(value)
                    self.dispatch_event(
                        'on_joystick_button', joystick, number,
                        joystick.button[number])

class SDLJoystickHandler(BaseJoystickHandler):
    # Poll SDL for joystick information, via ctypes. This is the
    # fallback if we don't have a better joystick driver for a
    # platform. (Currently, everything but Linux.)
    __SDL = None
    __devs = []

    __SDL_JOYSTICK_INIT = 1 << 9
    __SDL_ENABLE = 1

    @classmethod
    def init(cls, SDL):
        cls.__SDL = SDL

        SDL.SDL_JoystickOpen.restype = ctypes.c_void_p
        SDL.SDL_JoystickName.restype = ctypes.c_char_p
        SDL.SDL_JoystickGetAxis.restype = ctypes.c_short
        SDL.SDL_JoystickGetButton.restype = ctypes.c_byte

        SDL.SDL_Init(cls.__SDL_JOYSTICK_INIT)

    def _scan(self):
        # Close the device if it's open.
        try:
            for dev in self.__devs:
                self.__SDL.SDL_JoystickClose(dev)
        finally:
            self.__devs = []

        for number in range(self.__SDL.SDL_NumJoysticks()):
            # Open the SDL joystick for the given number.
            dev = self.__SDL.SDL_JoystickOpen(number)
            if dev:
                self.state.append(
                    Joystick(number,
                             self.__SDL.SDL_JoystickName(number),
                             self.__SDL.SDL_JoystickNumAxes(dev),
                             self.__SDL.SDL_JoystickNumButtons(dev)))
                self.__devs.append(dev)

    def dispatch_events(self):
        # Pump the SDL event queue.
        self.__SDL.SDL_JoystickUpdate()

        for joystick, dev in zip(self.state, self.__devs):
            # Check the state and compare it to our last one, fire events
            # for anything that's changed.
            for axis in range(joystick.axes):
                value = self.__SDL.SDL_JoystickGetAxis(dev, axis)
                value = value / 32768.0
                if value != joystick.axis[axis]:
                    joystick.axis[axis] = value
                    self.dispatch_event(
                        'on_joystick_axis', joystick, axis,
                        joystick.axis[axis])

            for button in range(joystick.buttons):
                value = bool(self.__SDL.SDL_JoystickGetButton(dev, button))
                if value != joystick.button[button]:
                    joystick.button[button] = value
                    self.dispatch_event(
                        'on_joystick_button', joystick, button,
                        joystick.button[button])

    def __del__(self):
        # Close the devices when we go out of scope.
        for dev in self.__devs:
            try: self.__SDL.SDL_JoystickClose(dev)
            except Exception: pass
        self.__devs = []

JoystickHandler = BaseJoystickHandler

if sys.platform == 'linux2':
    JoystickHandler = LinuxJoystickHandler
else:
    try:
        import ctypes
        import ctypes.util
    except ImportError:
        pass
    else:
        sdls = ["SDL", "SDL-1.2"]
        for sdl in sdls:
            path = ctypes.util.find_library(sdl)
            if path:
                SDL = getattr(ctypes.cdll, path)
                if SDL:
                    SDLJoystickHandler.init(SDL)
                    JoystickHandler = SDLJoystickHandler
                    break

# If run directly, do some debugging tests.
if __name__ == "__main__":
    j = JoystickHandler()
    print "%d joystick(s) found" % j.joysticks

    @j.event
    def on_joystick_button(joystick, button, pressed):
        print joystick

    @j.event
    def on_joystick_axis(joystick, axis, value):
        print joystick

    while True:
        j.dispatch_events()
