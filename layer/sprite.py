# $Id: sprite.py 450 2009-04-30 06:16:33Z joe.wreschnig $

import pyglet.sprite
import pyglet.graphics
import pyglet.gl
import layer.load
from math import sin, cos
import ctypes

c_float4 = ctypes.c_float * 4

Group = pyglet.graphics.Group
OrderedGroup = pyglet.graphics.OrderedGroup

class Batch(pyglet.graphics.Batch):

    GroupType = pyglet.graphics.OrderedGroup
    
    def __init__(self, *args, **kwargs):
        self.__groups = {}
        self.z = kwargs.pop("z", 0)
        super(Batch, self).__init__(*args, **kwargs)
    
    def Group(self, z):
        """Get a group for a given Z depth."""
        if z not in self.__groups:
            self.__groups[z] = self.GroupType(z)
        return self.__groups[z]

batch = default_batch = Batch()

def colorclamp(c):
    return (clamp(int(c[0]), 0, 255),
            clamp(int(c[1]), 0, 255),
            clamp(int(c[2]), 0, 255))

class Sprite(pyglet.sprite.Sprite):
    _scale_x = 1.0
    _scale_y = 1.0
    filename = None
    white_tex = pyglet.image.SolidColorImagePattern(
        (255, 255, 255, 255)).create_image(4, 4)

    _rgb4 = [(255, 255, 255)] * 4
    _opacity4 = [255] * 4
    _flip_x = False
    _flip_y = False
    
    def __init__(self, image=None, filename=None, batch=-1, group=-1,
                 z=0.0, **kwargs):
        """Create a new sprite, in a default batch/group."""
        # default batch is -1 because batch=None implies no batch.
        if batch == -1:
            batch = default_batch
        if group == -1 and batch is not None:
            group = batch.Group(z)
        if group == -1:
            group = None
        if image is None and filename is None:
            raise TypeError("Sprites need an image or filename")
        if filename == "white" and image is None:
            image = self.white_tex
            self.filename = "white"
        if filename is not None and "." not in filename:
            filename = filename + ".png"
        if image is None:
            image = layer.load.image(filename)
            self.filename = filename
        opacity = kwargs.pop("opacity", None)
        super(Sprite, self).__init__(image, batch=batch, group=group, **kwargs)
        if image is not None:
            tex = image.get_texture()
            pyglet.gl.glTexParameteri(pyglet.gl.GL_TEXTURE_2D,
                                      pyglet.gl.GL_TEXTURE_WRAP_S,
                                      pyglet.gl.GL_CLAMP_TO_EDGE)
            pyglet.gl.glTexParameteri(pyglet.gl.GL_TEXTURE_2D,
                                      pyglet.gl.GL_TEXTURE_WRAP_T,
                                      pyglet.gl.GL_CLAMP_TO_EDGE)
        self._z = z
        if opacity is not None:
            self.opacity = opacity

    def _get_opacity(self):
        return self._opacity4[0]

    def _set_opacity(self, o):
        self.opacity4 = [o, o, o, o]

    opacity = property(_get_opacity, _set_opacity)

    def _get_opacity4(self):
        return self._opacity4
    
    def _set_opacity4(self, (o1, o2, o3, o4)):
        self._opacity4 = [clamp(int(o1), 0, 255),
                          clamp(int(o2), 0, 255),
                          clamp(int(o3), 0, 255),
                          clamp(int(o4), 0, 255)]
        self._update_color()

    opacity4 = property(_get_opacity4, _set_opacity4)
            
    def _update_color(self):
        for i in range(4):
            r, g, b = self._rgb4[i]
            a = self._opacity4[i]
            self._vertex_list.colors[i*4:(i+1)*4] = [r, g, b, a]

    def _set_color(self, rgb):
        self.color4 = rgb, rgb, rgb, rgb

    def _get_color(self):
        return self._rgb4[0]

    color = property(_get_color, _set_color)

    def _set_color4(self, (c1, c2, c3, c4)):
        new_rgb = (colorclamp(c1),
                   colorclamp(c2),
                   colorclamp(c3),
                   colorclamp(c4))
        if self._rgb4 != new_rgb:
            self._rgb4 = new_rgb
            self._update_color()

    def _get_color4(self):
        return self._rgb4

    color4 = property(_get_color4, _set_color4)

    def _get_z(self):
        return self._z

    def _set_z(self, z):
        old_z = self._z
        if self.group and self.batch and self.group != self.batch.Group(old_z):
            raise ValueError("Sprite is not in Z ordering groups, can't set Z")
        else:
            if self.batch:
                self.group = self.batch.Group(z)
            self._z = z
    z = property(_get_z, _set_z, doc="Sprite Z order")

    def _set_scale(self, scale):
        self._scale_x = scale
        self._scale_y = scale
        self._update_position()
    scale = property(lambda self: max(self._scale_x, self._scale_y),
                     _set_scale,
                     doc="X/Y scale (max if different)")

    def _set_scale_x(self, scale_x):
        self._scale_x = scale_x
        self._update_position()
    scale_x = property(lambda self: self._scale_x,
                       _set_scale_x,
                       doc="X scale")

    def _set_scale_y(self, scale_y):
        self._scale_y = scale_y
        self._update_position()
    scale_y = property(lambda self: self._scale_y,
                       _set_scale_y,
                       doc="Y scale")

    def _set_flip_x(self, flip_x):
        self._flip_x = flip_x
        self._update_position()
    flip_x = property(lambda self: self._flip_x, _set_flip_x)
    
    def _set_flip_y(self, flip_y):
        self._flip_y = flip_y
        self._update_position()
    flip_y = property(lambda self: self._flip_y, _set_flip_y)

    def __set_width(self, width):
        self._scale_x = float(width) / self.image.width
        self._update_position()

    def __set_height(self, height):
        self._scale_y = float(height) / self.image.height
        self._update_position()

    width = property(lambda self: self.image.width * self._scale_x,
                     __set_width)
    height = property(lambda self: self.image.height * self._scale_y,
                      __set_height)

    def __get_position(self): return (self._x, self._y)
    def __set_position(self, (x, y)):
        self._x = x
        self._y = y
        self._update_position()
    position = property(__get_position, __set_position)

    def __get_xyr(self): return (self._x, self._y, self._rotation)
    def __set_xyr(self, (x, y, r)):
        self._x = x
        self._y = y
        self._rotation = r
        self._update_position()
    xyr = property(__get_xyr, __set_xyr)

    def _update_position(self):
        img = self._texture
        if not self._visible:
            self._vertex_list.vertices[:] = [0, 0, 0, 0, 0, 0, 0, 0]
        elif self._rotation:
            x1 = -img.anchor_x * self._scale_x
            y1 = -img.anchor_y * self._scale_y
            x2 = x1 + img.width * self._scale_x
            y2 = y1 + img.height * self._scale_y
            x = self._x
            y = self._y

            if self._flip_x:
                x1, x2 = x2, x1
            if self._flip_y:
                y1, y2 = y2, y1

            cr = cos(self._rotation)
            sr = sin(self._rotation)
            ax = int(x1 * cr - y1 * sr + x)
            ay = int(x1 * sr + y1 * cr + y)
            bx = int(x2 * cr - y1 * sr + x)
            by = int(x2 * sr + y1 * cr + y)
            cx = int(x2 * cr - y2 * sr + x)
            cy = int(x2 * sr + y2 * cr + y)
            dx = int(x1 * cr - y2 * sr + x)
            dy = int(x1 * sr + y2 * cr + y)

            self._vertex_list.vertices[:] = [
                ax, ay, bx, by, cx, cy, dx, dy]
        else:
            x1 = int(self._x - img.anchor_x * self._scale_x)
            y1 = int(self._y - img.anchor_y * self._scale_y)
            x2 = int(x1 + img.width * self._scale_x)
            y2 = int(y1 + img.height * self._scale_y)
            if self._flip_x:
                x1, x2 = x2, x1
            if self._flip_y:
                y1, y2 = y2, y1
            self._vertex_list.vertices[:] = [
                x1, y1, x2, y1, x2, y2, x1, y2]
