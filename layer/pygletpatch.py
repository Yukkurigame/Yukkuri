# $Id$

"""Monkeypatching for pyglet's interfaces.

Separated from layer.monkeypatch to avoid importing pyglet as early.

"""

# Apparently Pyglet never actually tested changing groups.
# http://code.google.com/p/pyglet/issues/detail?id=381
def migrate(self, vertex_list, mode, group, batch):
    formats = vertex_list.domain._Batch__formats
    domain = batch._get_domain(False, mode, group, formats)
    vertex_list.migrate(domain)
    vertex_list.domain._Batch__formats = formats    
import pyglet.graphics
pyglet.graphics.Batch.migrate = migrate

# Ridiculously, static sources don't have a duration.
def get_duration(self):
    if self._duration is None:
        self._duration = self._get_queue_source()._duration
    return self._duration
import pyglet.media
pyglet.media.StaticSource._get_duration = get_duration

