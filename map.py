#!/usr/bin/python

import math


from OpenGL.GL import *
from OpenGL.GLU import *
from PyQt4 import QtGui, QtCore
from PyQt4.QtOpenGL import *
from files import *
from framework import *
from graphics import GetGLTexture, CreateGLSprite, DrawGlSprite

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s


class Map(QGLWidget):
    '''
    Widget for drawing two spirals.
    '''
    
    def __init__(self, parent):
        QGLWidget.__init__(self, parent)
        self.setMinimumSize(640, 480)
        self.__Sprites = []

    def paintGL(self):
        '''
        Drawing routine
        '''
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)        
        for sprite in self.__Sprites:
            glLoadIdentity()
            DrawGlSprite(sprite)
        glFlush()

    def resizeGL(self, w, h):
        if h == 0: h = 1    # Prevent A Divide By Zero If The Window Is Too Small
        glViewport(0, 0, w, h)  # Reset The Current Viewport And Perspective Transformation
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()        
        gluPerspective(0, float(w)/float(h), 0.1, 100.0)
        glMatrixMode(GL_MODELVIEW)
    
    def CreateNewSprite(self, x, y, z, offsetx, offsety, w, h, name):
        self.__Sprites.append(CreateGLSprite(x, y, z, offsetx, offsety, w, h,
                            GetGLTexture(os.path.join(config.path, '..', 'images', name))))
    
    def initializeGL(self):
        w, h = 800, 600
        glViewport(0, 0, w, h)
        glEnable(GL_TEXTURE_2D) # Enable texture mapping.
        glClearColor(0.25, 0.43, 0.0, -1.0) # This Will Clear The Background Color To Black
        glClearDepth(1.0)   # Enables Clearing Of The Depth Buffer
        glShadeModel(GL_SMOOTH) # Enables Smooth Color Shading
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()    # Reset The Projection Matrix
        gluPerspective(0, w/h, 0.1, 100.0)   # Calculate The Aspect Ratio Of The Window
        glMatrixMode(GL_MODELVIEW)
        # setup blending
        glEnable(GL_BLEND)  # Enable Blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        
        
class MapWindow(QtGui.QMainWindow):
    
    def __init__(self, parent):
        #doing it wrong?
        self.__Parent = parent
        QtGui.QMainWindow.__init__(self)
        self.setWindowTitle(QtGui.QApplication.translate("Editor", "Map", None, QtGui.QApplication.UnicodeUTF8))
        self.__widget = Map(self)
        self.setCentralWidget(self.__widget)
        self.__BrushButton = None
        self.__Tiles = {}
    
    def LoadRegion(self, region):
        self.ReloadObjects()
        if not self.__Tiles: return
        for tile in region['tiles']:
            x, y = 0, 0
            if tile.has_key('x'): x = tile['x']
            if tile.has_key('y'): y = tile['y']
            if not tile.has_key('type'):
                print "Bida! Tile at "+str(x)+":"+str(x)+" has no type!"
                continue
            if not self.__Tiles.has_key(tile['type']):
                print "Bida! Tile at "+str(x)+":"+str(x)+" has non-declared or bad type " + tile['type'] 
                continue
            tiledata = self.__Tiles[tile['type']] 
            offsetx, offsety = 0, 0
            if tiledata.has_key('offsetx'): offsetx = int(tiledata['offsetx'])
            if tiledata.has_key('offsety'): offsety = int(tiledata['offsety'])
            add = 0
            if y % 2: add = 32
            x = round(x*64+add)
            y = round(y*64*0.3)
            self.__widget.CreateNewSprite(x, y, 0, offsetx, offsety, 64, 64*0.6, tiledata['image'])
        self.__widget.paintGL()
        self.show()

    def ReloadObjects(self):        
        files = filesManager.getFilesList('tiles')
        self.__Tiles.clear()
        row, col, maxcol = 0, 0, int(self.__Parent.ui.MapTilesPage.width()/64)  
        for file in files:
            data = lua.load(os.path.join(config.path, str(file)))
            if data[0] != 'Tiles': continue
            for tile in data[1:]:
                if type(tile).__name__ == 'dict' and tile.has_key('name') and tile.has_key('image'):
                    self.__Tiles[tile['name']] = tile
        for tile in self.__Tiles.values():
            x, y = 0, 0
            if tile.has_key('offsetx'): x = int(tile['offsetx'])
            if tile.has_key('offsety'): y = int(tile['offsety'])            
            image = CreatePixmap(tile['image'], x, y)
            if not image: continue
            l = QtGui.QPushButton(self.__Parent.ui.MapTilesPage)
            l.setFlat(1)
            l.setCheckable(1)
            l.setIconSize(QtCore.QSize(image.width(), image.height()))
            l.setIcon(QtGui.QIcon(image))
            l.setObjectName(_fromUtf8('Tile'+str(tile['name'])))
            self.connect(l, QtCore.SIGNAL("clicked()"), self.ChangeBrush)
            self.__Parent.ui.MapTilesPage.children()[0].addWidget(l, row, col)
            col += 1
            if col > maxcol: row += 1 
            
    def ChangeBrush(self):
        button = self.sender()
        for child in self.__Parent.ui.MapTilesPage.children()[1:]:
            if child != button: child.setChecked(0)
        if not button.isChecked(): button = None
        self.__BrushButton = button
