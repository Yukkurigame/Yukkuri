import math

from PyQt4 import QtGui, QtCore
from files import *
from framework import *

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Map(QtGui.QWidget):
    '''
    Widget for drawing two spirals.
    '''

    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        self.thisStatusBar = parent.thisStatusBar
        self.setMinimumSize(640, 480)
        self.setMouseTracking(1)
        self.__Sprites = []
        self.__translate = {'x': 0, 'y': 0}
        self.__position = {'x': 0, 'y': 0}
        self.__scale = 1.0
        self.__brush = None
        self.__draw = False

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.translate(self.__translate['x'], self.__translate['y'])
        painter.scale(self.__scale, self.__scale)
        painter.drawPoint(0,0)
        for sprite in self.__Sprites:
            painter.drawPixmap(sprite['position'], sprite['image'])

    def keyPressEvent(self, event):
        key = event.key()
        if key == QtCore.Qt.Key_Left:
            self.__translate['x'] -= 64
        elif key == QtCore.Qt.Key_Right:
            self.__translate['x'] += 64
        elif key == QtCore.Qt.Key_Down:
            self.__translate['y'] -= 64
        elif key == QtCore.Qt.Key_Up:
            self.__translate['y'] += 64
        elif key == QtCore.Qt.Key_Plus:
            self.__scale += 0.1
        elif key == QtCore.Qt.Key_Minus:
            self.__scale -= 0.1
        self.thisStatusBar.showMessage(str(int(self.__position['x']))+":"+str(int(self.__position['y']))
                                         + " Zoom: " + str(self.__scale * 100) + "%")
        self.update()
        
    def mousePressEvent(self, event):
        self.__draw = True
        self.drawTile()
    
    def mouseReleaseEvent(self, event):
        self.__draw = False
        
    def mouseMoveEvent(self, event):
        x = event.x() / self.__scale - self.__translate['x']
        y = event.y() / self.__scale - self.__translate['y']        
        #if y % 2: add = 32
        x = round(x/64)
        y = round(y/(64*0.3))
        if self.__position['x'] != x or self.__position['y'] != y:
            self.thisStatusBar.showMessage(str(int(x))+":"+str(int(y)) + " Zoom: " + str(self.__scale * 100) + "%")
            self.__position['x'] = x
            self.__position['y'] = y
            self.moveBrush()
            if self.__draw: self.drawTile()
    
    def CreateNewSprite(self, x, y, offsetx, offsety, w, h, name):
        self.__Sprites.append({'position': QtCore.QRect(x, y, w, h),
                            'image': CreatePixmap(name, offsetx, offsety, w, h)})
    
    def ClearSprites(self):
        self.__Sprites = []
    
    def drawTile(self):
        if not self.__brush: return 
        x = self.__brush['position'].x()
        y = self.__brush['position'].y()
        parent = self.parentWidget()
        if not parent: return
        x = int(round(x/64))
        y = int(round(y/(64*0.3)))
        if not parent.mapRegion.has_key(x): parent.mapRegion[x] = {}        
        parent.mapRegion[x][y] = self.__brush['type']
        offsetx, offsety = 0, 0
        self.__Sprites.append({'position': QtCore.QRect(self.__brush['position']),
                            'image': QtGui.QPixmap(self.__brush['image'])})
        self.update()
    
    def setBrush(self, tile):
        x, y = 0, 0
        if not tile:
            if self.__brush: self.__Sprites.append(self.__brush)
            else: return                 
        else:
            if tile.has_key('offsetx'): x = int(tile['offsetx'])
            if tile.has_key('offsety'): y = int(tile['offsety'])
            self.__brush = {'position': QtCore.QRect(0, 0, 64, 64*0.6),
                            'image': CreatePixmap(tile['image'], x, y, 64, 64*0.6),
                            'type': tile['name']}
            self.__Sprites.append(self.__brush)
    
    def clearBrush(self):
        if not self.__brush: return
        self.__Sprites.remove(self.__brush)
        self.__brush = None
    
    def moveBrush(self):
        if not self.__brush: return
        x, y = self.__position['x'], self.__position['y']
        add = 0
        if y % 2: add = 32
        x = round(x*64+add)
        y = round(y*64*0.3)
        self.__brush['position'].moveTo(x, y)
        self.update()
        

class MapWindow(QtGui.QMainWindow):

    def __init__(self, parent):
        self.__Parent = parent
        QtGui.QMainWindow.__init__(self)
        self.setWindowTitle(QtGui.QApplication.translate("Editor", "Map", None, QtGui.QApplication.UnicodeUTF8))
        self.thisStatusBar = QtGui.QStatusBar()
        self.setStatusBar(self.thisStatusBar)
        self.__widget = Map(self)
        self.setCentralWidget(self.__widget)
        self.__BrushButton = None
        self.__Tiles = {}
        self.mapRegion = {}
        self.__regionName = None

    def keyPressEvent(self, event):
        self.__widget.keyPressEvent(event)
    
    def LoadRegion(self, region):
        self.hide()
        self.ReloadObjects()
        self.mapRegion.clear()        
        self.__widget.ClearSprites()
        self.__widget.setBrush(None)
        self.__regionName = region['name']
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
            if not self.mapRegion.has_key(x): self.mapRegion[x] = {}
            self.mapRegion[x][y] = tile['type']
            offsetx, offsety = 0, 0
            if tiledata.has_key('offsetx'): offsetx = int(tiledata['offsetx'])
            if tiledata.has_key('offsety'): offsety = int(tiledata['offsety'])            
            add = 0
            if y % 2: add = 32
            x = round(x*64+add)
            y = round(y*64*0.3)            
            self.__widget.CreateNewSprite(x, y, offsetx, offsety, 64, 64*0.6, tiledata['image'])
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
            tile['button'] = l
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
        if not button.isChecked():
            button = None
        self.__widget.clearBrush()            
        self.__BrushButton = button
        if not button: return
        for tile in self.__Tiles.values():
            if tile.has_key('button') and tile['button'] == button:
                self.__widget.setBrush(tile)
    
    def dump(self):
        self.hide()
        ret = {}        
        ret['name'] = self.__regionName
        ret['tiles'] = []
        for i in self.mapRegion:
            for j in self.mapRegion[i]:
                d = 0
                try: d = int(self.mapRegion[i][j])
                except: continue 
                if d != 0:
                    ret['tiles'].append({'x': i, 'y': j, 'type': d})
        return ret
