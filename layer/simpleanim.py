# $Id: simpleanim.py 284 2009-03-22 10:02:53Z piman $

import layer.colors

from layer.parse import ParseObject
from layer.task import Task

def _interp(a, b, p):
    if a is None:
        return b
    elif b is None:
        return a
    else: return a * (1 - p) + b * p

class SimpleKeyFrame(ParseObject):
    x = None
    y = None
    width = None
    height = None
    scale_x = None
    scale_y = None
    color = None
    duration = 0
    opacity = None
    rotation = None
    next = []

    def __init__(self, name=None, x=None, y=None,
                 scale_x=None, scale_y=None, color=None, opacity=None,
                 rotation=None, next=[], duration=0):
        self.name = name
        self.duration = duration
        self.next = list(next)
        self.x = x
        self.y = y
        self.scale_x = scale_x
        self.scale_y = scale_y
        self.color = color
        self.opacity = opacity
        self.rotation = rotation

    def set(self, sprite):
        if self.x is not None:
            sprite.x = self.x
        if self.y is not None:
            sprite.y = self.y

        if self.scale_x is not None:
            sprite.width = sprite.image.width * self.scale_x

        if self.scale_y is not None:
            sprite.height = sprite.image.height * self.scale_y

        if self.color is not None:
            sprite.color = self.color

        if self.opacity is not None:
            sprite.opacity = self.opacity

        if self.rotation is not None:
            sprite.rotation = self.rotation

    def merge(self, sprite):
        ret = SimpleKeyFrame(**self.__dict__)
        for key in ["x", "y",  "color", "opacity", "rotation",
                    "scale_x", "scale_y"]:
            if getattr(ret, key, None) is None:
                setattr(ret, key, getattr(sprite, key, None))
        return ret

    def interpolate(self, other, p, sprite):
        if p <= 0:
            self.set(sprite)
            return
        elif p >= 1:
            other.set(sprite)
            return

        x = _interp(self.x, other.x, p)
        if x is not None:
            sprite.x = x

        y = _interp(self.y, other.y, p)
        if y is not None:
            sprite.y = y

        scale_x = _interp(self.scale_x, other.scale_x, p)
        if scale_x is not None:
            sprite.scale_x = scale_x

        scale_y = _interp(self.scale_y, other.scale_y, p)
        if scale_y is not None:
            sprite.scale_y = scale_y

        if self.color and other.color:
            sprite.color = layer.colors.interpolate(self.color, other.color, p)
        elif other.color:
            sprite.color = other.color
        elif self.color:
            sprite.color = self.color

        opacity = _interp(self.opacity, other.opacity, p)
        if opacity is not None:
            sprite.opacity = opacity
            
        rotation = _interp(self.rotation, other.rotation, p)
        if rotation is not None:
            sprite.rotation = rotation

class SimpleAnimationSequence(ParseObject):
    load_spec = "*.anim"

    _shelves_ = ["transitions"]

    start = property(lambda self: self.transitions[0].name)

    def __getitem__(self, name):
        name = name.lower()
        for t in self.transitions:
            if t.name == name:
                return t
        raise KeyError

class SimpleAnimation(object):
    def __init__(self, sprite, seq):
        self._def = SimpleAnimationSequence.Find(seq)
        self._sprite = sprite
        self._time = 0
        self._duration = 0
        self._frame = None
        self._task = None

    def enable(self):
        if self._task:
            self.disable()
        self._time = 0
        self._index = 0
        self._last_frame = None
        self._frame = self._def[self._def.start].merge(self._sprite)
        self._duration = self._def[self._def.start].duration
        self._task = Task(self._update)
        self._task.enable()
        self._frame.interpolate(None, 0, self._sprite)

    def disable(self):
        if self._task:
            self._task.disable()
        self._task = None
        self._frame = None
        self._last_frame = None
        self._time = 0
        self._index = 0
        self._duration = 0

    def _update(self, dt):
        if self._frame is None:
            return
        self._time += dt
        if self._duration:
            p = min(1, self._time / self._duration)
        else:
            p = 1
        if self._last_frame:
            self._last_frame.interpolate(self._frame, p, self._sprite)
        else:
            self._frame.set(self._sprite)
        if p >= 1:
            self._last_frame = self._frame
            self._time = 0
            try:
                next = self._frame.next[0]
                f = self._def[next]
                self._frame = f.merge(self._sprite)
                self._duration = f.duration
            except (IndexError, KeyError):
                self._frame = None
                self._last_frame = None
