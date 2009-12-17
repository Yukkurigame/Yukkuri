# $Id: entity.py 464 2009-05-01 08:43:58Z joe.wreschnig $

from layer.task import Task
from layer.tracker import InstanceTracker

__all__ = ["EntityGroup", "Entity"]

class EntityGroup(list):
    _window = None
    task = None
    _period = 0
    
    def __init__(self, name=None):
        self.name = name

    def add(self, entity):
        try: index = self.index(entity)
        except ValueError:
            self.append(entity)
            try: entity.entgroups.append(self)
            except AttributeError:
                entity.entgroups = [self]
            return len(self) - 1
        else:
            return index

    def kill(self, entity):
        try: index = self.index(entity)
        except ValueError: return -1
        else:
            entity.entgroups.remove(self)
            self.pop(index)
            return index

    remove = kill

    def enable(self, upper_bound=1.0/30.0,
               tick_args=[], tick_kwargs={},
               draw_args=[], draw_kwargs={}, **kwargs):
        if self.task is not None:
            self.disable()
        kwargs.setdefault("period", 0)
        kwargs.setdefault("upper_bound", upper_bound)
        self.task = Task(self.tick, **kwargs)
        self.task.enable()
        self._tick_args = tick_args
        self._draw_args = draw_args
        self._tick_kwargs = tick_kwargs
        self._draw_kwargs = draw_kwargs        

    def _get_window(self):
        return self._window

    def _set_window(self, window):
        if self._window is not None:
            self._window.remove_handlers(on_draw=self.draw)
        self._window = window
        if self._window is not None:
            self._window.push_handlers(on_draw=self.draw)

    window = property(_get_window, _set_window)

    def disable(self):
        if self.task is not None:
            self.task.disable()
            self.task = None
            self._tick_args = []
            self._draw_args = []
            self._tick_kwargs = {}
            self._draw_kwargs = {}

    def destroy(self):
        self.disable()
        self.window = None
        for entity in list(self):
            entity.kill()

    def tick(self, dt):
        for entity in self:
            entity._ticked = True
            entity.tick(dt, *self._tick_args, **self._tick_kwargs)

    def draw(self):
        for entity in self:
            entity.draw(*self._draw_args, **self._draw_kwargs)
            entity._ticked = False

class Entity(object):
    _ticked = True

    def __init__(self, flags=[], groups=[]):
        super(Entity, self).__init__()
        self.flags = set(flags)
        self.entgroups = list(groups)

    def tick(self, dt):
        pass

    def draw(self):
        pass

    def kill(self):
        for group in list(self.entgroups):
            group.kill(self)
        try:
            self.sprite.batch = None
        except AttributeError: pass
