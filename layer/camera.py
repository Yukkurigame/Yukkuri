# $Id: camera.py 314 2009-04-02 04:20:41Z piman $

from layer.parse import ParseObject
from layer.task import Task
from layer.command import command

from math import cos, sin, atan2

from pyglet.gl import (
    glLoadIdentity, glMatrixMode, gluLookAt, gluOrtho2D,
    GL_MODELVIEW, GL_PROJECTION,
)

class CameraState(ParseObject):
    x = None
    y = None
    scale = None
    rotation = None
    offsetx = 0
    offsety = 0

    def __init__(self, x=None, y=None, scale=None, rotation=None,
                 offsetx=0, offsety=0):
        self.x = x
        self.y = y
        self.offsetx = offsetx
        self.offsety = offsety
        self.scale = scale
        self.rotation = rotation

    def __eq__(self, other):
        return (self.x == other.x
                and self.y == other.y
                and self.offsetx == other.offsetx
                and self.offsety == other.offsety
                and self.rotation == other.rotation
                and self.scale == other.scale)

    def merge(self, state):
        if state.x is not None:
            self.x = state.x
        if state.y is not None:
            self.y = state.y
        if state.offsetx is not None:
            self.offsetx = state.offsetx
        if state.offsety is not None:
            self.offsety = state.offsety
        if state.scale is not None:
            self.scale = state.scale
        if state.rotation is not None:
            self.rotation = state.rotation

    def interpolate_ip(self, state, p):
        q = 1 - p
        if state.x is not None:
            if self.x is None:
                self.x = state.x
            else:
                self.x = self.x * q + state.x * p
        if state.y is not None:
            if self.y is None:
                self.y = state.y
            else:
                self.y = self.y * q + state.y * p
        if state.scale is not None:
            if self.scale is None:
                self.scale = state.scale
            else:
                self.scale = self.scale * q + state.scale * p
        if state.rotation is not None:
            if self.rotation is None:
                self.rotation = state.rotation
            else:
                self.rotation = self.rotation * q + state.rotation * p

        if state.offsetx is not None:
            if self.offsetx is None:
                self.offsetx = state.offsetx
            else:
                self.offsetx = self.offsetx * q + state.offsetx * p

        if state.offsety is not None:
            if self.offsety is None:
                self.offsety = state.offsety
            else:
                self.offsety = self.offsety * q + state.offsety * p

class CameraTransition(ParseObject):
    _shelves_ = ["change", "begin", "duration"]

    end = property(lambda self: self.duration + self.begin)

class CameraScript(ParseObject):
    load_spec = "*.camscript"

    start = CameraState(0, 0, 1, 0)

    _thawers_ = dict(states=lambda l: list(sorted(l, key=lambda s: s.begin)))

    def fill_state(self, to_fill, time):
        done = True
        to_fill.merge(self.start)
        # if two transitions are animating the same property at the
        # same time, we get undefined results.
        for state in self.states:
            if time < state.begin:
                return False
            elif time >= state.end:
                to_fill.merge(state.change)
            else:
                done = False
                if time > state.begin:
                    p = (time - state.begin) / state.duration
                    to_fill.interpolate_ip(state.change, p)
        return done

class Camera(object):
    def __init__(self, view, world, x=0, y=0, scale=1, rotation=0):
        self.state = CameraState(x, y, scale, rotation)
        self.view = view
        self.world = world
        self.targets = []

    def push_target(self, target):
        try: index = self.targets.index(target)
        except ValueError:
            self.targets.append(target)
        else:
            self.targets.append(self.targets.pop(index))

    def pop_target(self, target):
        try: self.targets.remove(target)
        except ValueError: pass

    def _get_scale(self): return self.state.scale
    def _set_scale(self, scale): self.state.scale = scale
    scale = property(_get_scale, _set_scale)

    def _get_rotation(self): return self.state.rotation
    def _set_rotation(self, rotation): self.state.rotation = rotation
    rotation = property(_get_rotation, _set_rotation)

    def _get_x(self): return self.state.x
    def _set_x(self, x): self.state.x = x
    x = property(_get_x, _set_x)

    def _get_y(self): return self.state.y
    def _set_y(self, y): self.state.y = y
    y = property(_get_y, _set_y)

    def clamp(self):
        try: world_width, world_height = self.world.width, self.world.height
        except AttributeError:
            world_width, world_height = self.world
        try: view_width, view_height = self.view.width, self.view.height
        except AttributeError:
            view_width, view_height = self.view
        self.state.scale = min(
            self.state.scale, world_width / float(view_width))
        self.state.scale = min(
            self.state.scale, world_height / float(view_height))
        # translate view into world units.
        half_view_width = view_width / 2.0 * self.state.scale
        half_view_height = view_height / 2.0 * self.state.scale
        offsetx = self.state.offsetx or 0
        offsety = self.state.offsety or 0
        self.state.x = max(self.state.x + offsetx, half_view_width)
        self.state.x = min(self.state.x + offsetx, world_width - half_view_width)
        self.state.y = max(self.state.y + offsety, half_view_height)
        self.state.y = min(self.state.y + offsety, world_height - half_view_height)

    def focus(self):
        try: view_width, view_height = self.view.width, self.view.height
        except AttributeError:
            view_width, view_height = self.view
        # Set projection matrix suitable for 2D rendering
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(
            -self.state.scale * view_width / 2.0,
            +self.state.scale * view_width / 2.0,
            -self.state.scale * view_height / 2.0,
            +self.state.scale * view_height / 2.0)

        # Set modelview matrix to move, scale & rotate to camera position
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        offsetx = self.state.offsetx or 0
        offsety = self.state.offsety or 0
        gluLookAt(
            self.state.x + offsetx, self.state.y + offsety, +1.0,
            self.state.x + offsetx, self.state.y + offsety, -1.0,
            sin(self.state.rotation), cos(self.state.rotation), 0.0)

    def __update_script(self, dt, script, time):
        time[0] += dt
        return script.fill_state(self.state, time[0])

    def play_script(self, script):
        Task(self.__update_script, script, [0], stoppable=True, enabled=True)

_default_camera = None

def set_default_camera(cam):
    global _default_camera
    _default_camera = cam

@command(declspec=[float])
def camera_scale(scale=None):
    if _default_camera is None:
        print_e("No camera is set.")
    elif scale is not None:
        _default_camera.scale = scale
    else:
        return _default_camera.scale

@command(declspec=[float])
def camera_rotation(rotation=None):
    if _default_camera is None:
        print_e("No camera is set.")
    elif rotation is not None:
        _default_camera.rotation = rotation
    else:
        return _default_camera.rotation

@command(declspec=[str])
def camera_play_script(name):
    if _default_camera is None:
        print_e(_("No camera is set."))
    else:
        script = CameraScript.Find(name)
        if script is not None:
            print_(_("Running camera script %r.") % script)
            _default_camera.play_script(script)
        else:
            print_e(_("Camera script %r not found.") % script)
