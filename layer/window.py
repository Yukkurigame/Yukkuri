# $Id: window.py 453 2009-04-30 08:03:59Z joe.wreschnig $

import time
import pyglet
import layer.util
import layer.keybind
import layer.sprite
import layer.command
import layer.colors

from layer.joystick import JoystickHandler
from layer.console import Console
from layer.task import Task

windows = pyglet.app.windows

class MouseState(object):
    __slots__ = ["x", "y", "z", "dx", "dy", "dz", "state"]

    def __init__(self):
        self.state = [False, False, False]
        self.x = 0
        self.y = 0
        self.z = 0
        self.dx = 0
        self.dy = 0
        self.dz = 0

    def reset(self):
        self.dx = 0
        self.dy = 0
        self.dz = 0

    __repr__ = layer.util.repr_from_slots

class InputState(pyglet.event.EventDispatcher):
    __slots__ = ["key", "edge", "mouse", "joystick"]
    debug = False

    @staticmethod
    def joy_button_to_key(joystick, button):
        return "joy%d,button%d" % (joystick, button)

    @staticmethod
    def joy_axis_to_key(joystick, axis):
        return "joy%d,axis%d" % (joystick, axis)

    @staticmethod
    def mouse_buttons_to_keys(button_mask):
        button = 0
        buttons = []
        while button_mask > 0:
            if button_mask & 1:
                buttons.append("mouse%d" % (button + 1))
            button += 1
            button_mask >>= 1
        return buttons

    def __init__(self, window):
        self.__key = dict()
        self.mouse = MouseState()
        self.joystick = JoystickHandler()
        self.joystick.push_handlers(self)
        window.push_handlers(self)
        layer.keybind.init(self)
        layer.keybind.push("default")
        self.__task = Task(lambda dt: self.joystick.dispatch_events(),
                           enabled=True)

    def __getitem__(self, key):
        try: key = key.lower()
        except AttributeError:
            key = pyglet.window.key.symbol_string(key)
        try: return self.__key[key]
        except KeyError:
            return False

    def _press(self, key, vars={}, expect_release=True):
        key = key.lower()
        self.__key[key] = True
        if self.debug:
            print_d("Press: %r, vars=%r" % (key, vars))
        self.dispatch_event("on_input_press", self, key, vars)
        if not expect_release:
            self.__key[key] = False

    def _release(self, key, vars={}):
        key = key.lower()
        self.__key[key] = False
        if self.debug:
            print_d("Release: %r, vars=%r" % (key, vars))
        self.dispatch_event("on_input_release", self, key, vars)

    def on_mouse_motion(self, x, y, dx, dy):
        self.mouse.x = x
        self.mouse.y = y
        self.mouse.dx += dx
        self.mouse.dy += dy
        self._press("mousemotion", dict(x=x, y=y, dx=dx, dy=dy), False)

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        self.mouse.x = x
        self.mouse.y = y
        self.mouse.dx += dx
        self.mouse.dy += dy
        self._press("mousemotion", dict(x=x, y=y, dx=dx, dy=dy), False)

    def on_mouse_press(self, x, y, button_mask, modifiers):
        d = dict(x=x, y=y)
        for button in self.mouse_buttons_to_keys(button_mask):
            self._press(button, d)

    def on_mouse_release(self, x, y, button_mask, modifiers):
        d = dict(x=x, y=y)
        for button in self.mouse_buttons_to_keys(button_mask):
            self._release(button, d)

    def on_mouse_scroll(self, x, y, scroll_x, scroll_y):
        if scroll_y:
            self.mouse.dz = scroll_y
            self.mouse.z += scroll_y
            self._press("mousewheel", dict(x=x, y=y), False)

    def on_key_press(self, symbol, modifiers):
        self._press(pyglet.window.key.symbol_string(symbol))
        return pyglet.event.EVENT_HANDLED

    def on_key_release(self, symbol, modifiers):
        self._release(pyglet.window.key.symbol_string(symbol))
        return pyglet.event.EVENT_HANDLED

    def on_joystick_button(self, joystick, button, pressed):
        if pressed:
            self._press(self.joy_button_to_key(joystick.number, button))
        else:
            self._release(self.joy_button_to_key(joystick.number, button))

    def reset(self):
        self.mouse.reset()

InputState.register_event_type("on_input_press")
InputState.register_event_type("on_input_release")

class Window(pyglet.window.Window):
    __fps = None

    def __init__(self, *args, **kwargs):
        self.__clippers = [None]
        super(Window, self).__init__(*args, **kwargs)
        self.input = InputState(self)
        self.batches = [layer.sprite.default_batch]
        self.console = Console(self)
        self.fps = layer.DEBUG
        self.bg_color = "grey20"
        self.screenshots = []
        self.screenshot_delay = 0

    def _fps(self, onoff):
        if not onoff and self.__fps:
            self.__fps.unschedule()
            self.__fps = None
        elif onoff and not self.__fps:
            self.__fps = pyglet.clock.ClockDisplay()

    fps = property(lambda self: bool(self.__fps), _fps)

    def restore_state(self):
        pyglet.gl.glMatrixMode(pyglet.gl.GL_PROJECTION)
        pyglet.gl.glLoadIdentity()
        pyglet.gl.gluOrtho2D(0, self.width, 0, self.height)
        pyglet.gl.glMatrixMode(pyglet.gl.GL_MODELVIEW)
        pyglet.gl.glLoadIdentity()

    def __get_bg_color(self):
        return self.__bg_color

    def __set_bg_color(self, color):
        self.__bg_color = color
        r, g, b = layer.colors.value(color)
        pyglet.gl.glClearColor(r / 255., g / 255., b / 255., 1.0)

    bg_color = property(__get_bg_color, __set_bg_color)

    def on_draw(self):
        self.clear()
        self.batches.sort(key=lambda b: getattr(b, "z", 0))
        for batch in self.batches:
            batch.draw()
        if not self.screenshot_delay and self.screenshots:
            for filename in self.screenshots:
                cbuffer = pyglet.image.get_buffer_manager().get_color_buffer()
                cbuffer.save(filename)
            self.screenshots = []
        else:
            self.screenshot_delay = 0

        if self.__fps:
            self.__fps.draw()
        self.input.reset()

    def screenshot(self, filename="screenshot-%(time)s.png"):
        self.screenshot_delay = 2
        t = time.strftime("%F-%H.%M.%S", time.localtime())
        filename = filename % dict(time=t)
        self.screenshots.append(filename)

    vsync = property(lambda self: self._vsync,
                     pyglet.window.Window.set_vsync,
                     doc="Vertical refresh synchronization.")
