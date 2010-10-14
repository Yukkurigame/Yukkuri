from Image import *
from OpenGL.GL import *

class Sprite:
    def __init__(self):
        self.tex = None
        self.vertices = None
        self.coordinates = None
        self.clr = None
        self.width = 0.0
        self.height = 0.0
        self.posx = 0.0
        self.posy = 0.0
        self.visible = True
        self.centered = False

    def resize(self, w, h):
        if not self.vertices: return
        if w >= 0:
            self.vertices.rb.x = self.vertices.rt.x = self.vertices.lb.x + w
            self.width = w
        if h >= 0:
            self.vertices.rt.y = self.vertices.lt.y = self.vertices.lb.y + h
            height = h

    def setPosition(self, x, y, z = None):
        if not self.vertices: return
        self.width = self.vertices.rb.x - self.vertices.lb.x
        self.height = self.vertices.rt.y - self.vertices.lb.y
        self.posx = x
        self.posy = y
        if self.centered:
            halfwidth = self.width/2
            halfheight = self.height/2
            self.vertices.lb.x = self.vertices.lt.x = x - halfwidth
            self.vertices.lb.y = self.vertices.rb.y = y - halfheight
            self.vertices.rb.x = self.vertices.rt.x = x + halfwidth
            self.vertices.lt.y = self.vertices.rt.y = y + halfheight
        else:
            self.vertices.lb.x = self.vertices.lt.x = x
            self.vertices.lb.y = self.vertices.rb.y = y
            self.vertices.rb.x = self.vertices.rt.x = x + width
            self.vertices.lt.y = self.vertices.rt.y = y + height
        if z: self.vertices.z = z

    def toggleVisibility(self):
        if self.visible: self.visible = False
        else: self.visible = True

class Texture:
    def __init__(self):
        self.texture = None
        self.w = 0
        self.h = 0

class vertex3farr:
    def __init__(self):
       self.lt = s2f()
       self.lb = s2f()
       self.rt = s2f()
       self.rb = s2f()
       self.z = 0

class s2f:
    def __init__(self):
       self.x = 0
       self.y = 0

class Color:
    def __init__(self):
        self.r = 255
        self.b = 255
        self.g = 255
        self.a = 255

def DrawGlSprite(sprite):
    vertices = sprite.vertices
    coordinates = sprite.coordinates

    glBindTexture(GL_TEXTURE_2D, sprite.tex.texture)

    glColor4ub(sprite.clr.r, sprite.clr.g, sprite.clr.b, sprite.clr.a)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0) 
    glVertex3f(-1.0,-1.0, 0.0)
    glTexCoord2f(1.0, 0.0) 
    glVertex3f( 1.0,-1.0, 0.0)
    glTexCoord2f(1.0, 1.0) 
    glVertex3f( 1.0, 1.0, 0.0)
    glTexCoord2f(0.0, 1.0) 
    glVertex3f(-1.0, 1.0, 0.0)
    glEnd()
    #if coordinates:
        #glBegin(GL_QUADS)

        #Bottom-left vertex (corner)
        #glTexCoord2f(coordinates.lb.x, coordinates.lb.y)
        #glVertex3f( vertices.lb.x, vertices.lb.y, vertices.z)

        #Bottom-right vertex (corner)
        #glTexCoord2f(coordinates.rb.x , coordinates.rb.y)
        #glVertex3f(vertices.rb.x, vertices.rb.y, vertices.z)

        #Top-right vertex (corner)
        #glTexCoord2f(coordinates.rt.x , coordinates.rt.y)
        #glVertex3f(vertices.rt.x, vertices.rt.y, vertices.z)

        #Top-left vertex (corner)
        #glTexCoord2f(coordinates.lt.x , coordinates.lt.y)
        #glVertex3f(vertices.lt.x, vertices.lt.y, vertices.z)

        #glEnd()
    #else:
        #glBegin( GL_QUADS ) #draw rectangle
        #glVertex3f(vertices.lb.x, vertices.lb.y, vertices.z) #Bottom-left
        #glVertex3f(vertices.rb.x, vertices.rb.y, vertices.z) #Bottom-right
        #glVertex3f(vertices.rt.x, vertices.rt.y, vertices.z) #Top-right
        #glVertex3f(vertices.lt.x, vertices.lt.y, vertices.z) #Top-left
        #glEnd()

def CreateGLSprite(x, y, z, texX, texY, width, height, tex, centered = 1):

    sprite = Sprite()

    sprite.vertices = GetVertex( x, y, z, width, height, centered )
    sprite.width = width
    sprite.height = height
    sprite.clr = Color()

    sprite.coordinates = GetCoordinates(texX, texY, width, height, tex.w, tex.h)
    sprite.tex = tex

    if centered: sprite.centered = True

    return sprite

def GetVertex(x, y, z, width, height, centered):

    v = vertex3farr()
    xp, xm, yp, ym = 0, 0, 0, 0

    if centered:
        halfwidth = width/2
        halfheight = height/2
        xp = x + halfwidth
        xm = x - halfwidth
        yp = y + halfheight
        ym = y - halfheight
    else:
        xp = x + width
        xm = x
        yp = y + height
        ym = y

    v.lb.x = xm
    v.lb.y = ym
    v.lt.x = xm
    v.lt.y = yp
    v.rb.x = xp
    v.rb.y = ym
    v.rt.x = xp
    v.rt.y = yp
    v.z = z

    return v

def GetCoordinates(x1, y1, x2, y2, width, height):    

    if x1 == 0 and x2 == 0 and y1 == 0 and y2 == 0: return
    c = vertex3farr()

    cx1 = x1 / width
    cx2 = ( x1 + x2 ) / width
    cy2 = y1 / height
    cy1 = ( y1 + y2 ) / height

    c.lt.x = cx1
    c.lt.y = cy2
    c.lb.x = cx1
    c.lb.y = cy1
    c.rt.x = cx2
    c.rt.y = cy2
    c.rb.x = cx2
    c.rb.y = cy1
    return c

def GetGLTexture(name):

    if not name: return

    texture = None

    if name in LoadedTextures:
        texture = LoadedTextures[name]
    else:
        texture = Texture()
        im = open(name)
        ix = im.size[0]
        iy = im.size[1]
        image = None
        try:
            # get image meta-data (dimensions) and data
            image = im.tostring("raw", "RGBA", 0, -1)
        except SystemError:
            # has no alpha channel, synthesize one, see the
            # texture module for more realistic handling
            image = im.tostring("raw", "RGBX", 0, -1)

        tex = range(1)
        tex = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, tex)   # 2d texture (x and y size)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR) # scale linearly when image bigger than texture
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR) # scale linearly when image smalled than texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, image)

        texture.texture = tex
        texture.w = ix
        texture.h = iy

        LoadedTextures[name] = texture

    return texture

LoadedTextures = {}
