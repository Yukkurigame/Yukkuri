import math

from PyQt4 import QtGui, QtCore
from config import config
from files import *
from framework import *
from sprites import manager as sprites

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

def fromMapPosition(x, y):
    x = int(x) << 5
    y = int(y) << 5
    mx = x - y
    my = x/2 + y/2
    return mx, -my

def toMapPosition(mx, my):
    my = -my
    x = (2 * my + mx)/2
    y = (2 * my - mx)/2
    x = int(x) >> 5
    y = int(y) >> 5
    return x, y

def compareSprites(one, two):
    if one.z or two.z:
        return cmp(one.z, two.z)
    if one.y == two.y:
        return cmp(one.x, two.x)
    return cmp(one.y, two.y)

class Tile(dict):
    def __init__(self, data=None):
        if data:
            dict.__init__(self, data)
    def __getattr__(self, item):
        try:
            return self.__getitem__(item)
        except KeyError:
            return 0
    def __setattr__(self, item, value):
        if self.__dict__.has_key(item):
            dict.__setattr__(self, item, value)
        else:
            self.__setitem__(item, value)

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
        self.__keys = {}

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.translate(self.__translate['x'], self.__translate['y'])
        painter.scale(self.__scale, self.__scale)
        painter.drawPoint(0,0)
        self.__Sprites.sort(lambda x, y: compareSprites(x, y))
        for sprite in self.__Sprites:
            painter.drawPixmap(sprite.x - 32, sprite.y - 64, sprite.width,
                                sprite.height, sprite.image)

    def keyPressEvent(self, event):
        key = event.key()
        if key == QtCore.Qt.Key_Left:
            self.__translate['x'] += 64
        elif key == QtCore.Qt.Key_Right:
            self.__translate['x'] -= 64
        elif key == QtCore.Qt.Key_Down:
            self.__translate['y'] -= 64
        elif key == QtCore.Qt.Key_Up:
            self.__translate['y'] += 64
        elif key == QtCore.Qt.Key_Plus and self.__scale < 9.9:
            self.__scale += 0.1
        elif key == QtCore.Qt.Key_Minus and self.__scale > 0.2:
            self.__scale -= 0.1
        elif key == QtCore.Qt.Key_Shift and self.__brush:
            self.__brush.rectangle = {'x': self.__position['x'], 'y':int(self.__position['y'])}
        self.thisStatusBar.showMessage(str(int(self.__position['x']))+":"+str(int(self.__position['y']))
                                         + " Zoom: " + str(self.__scale * 100) + "%")
        self.update()

    def keyReleaseEvent(self, event):
        key = event.key()
        if key == QtCore.Qt.Key_Shift and self.__brush:
            self.__brush.rectangle = None

    def mousePressEvent(self, event):
        self.__draw = True

    def mouseReleaseEvent(self, event):
        self.__draw = False
        self.drawTile()

    def wheelEvent(self, event):
        numDegrees = event.delta() / 8;
        numSteps = numDegrees / 15;

        if QtGui.QApplication.keyboardModifiers() & QtCore.Qt.AltModifier:
            if ( numSteps < 0 and self.__scale > 0.2 ) or self.__scale < 9.9:
                self.__scale += 0.1 * numSteps
        elif event.orientation() == QtCore.Qt.Horizontal or \
            QtGui.QApplication.keyboardModifiers() & QtCore.Qt.ControlModifier:
            self.__translate['x'] -= 64 * numSteps
        else:
            self.__translate['y'] += 64 * numSteps

        self.thisStatusBar.showMessage(str(int(self.__position['x']))+":"+str(int(self.__position['y']))
                                         + " Zoom: " + str(self.__scale * 100) + "%")
        self.update()

    def mouseMoveEvent(self, event):
        if not self.__brush: return
        x = (event.x() - self.__translate['x']) / self.__scale
        y = (event.y() - self.__translate['y']) / self.__scale
        x, y = toMapPosition(x, y)
        if self.__position['x'] != x or self.__position['y'] != y:
            self.thisStatusBar.showMessage(str(int(x))+":"+str(int(y)) + " Zoom: " + str(self.__scale * 100) + "%")
            self.__position['x'] = x
            self.__position['y'] = y
            self.moveBrush()

    def CreateNewSprite(self, x, y, z, tile):
        s = sprites.createPixmap(tile.image, tile.picture)
        sprite = Sprite(QtCore.QRect(x, y, s.width(), s.height()), s, depth=z)
        self.__Sprites.append(sprite)
        return sprite

    def ClearSprites(self):
        self.__Sprites = []

    def drawTile(self):
        if not self.__brush:
            return
        parent = self.parentWidget()
        if not parent:
            return
        for tile in self.__brush.sprites:
            x = tile.x
            y = tile.y
            x, y = toMapPosition(x, y)
            if not parent.mapRegion.has_key(x):
                parent.mapRegion[x] = {}
            if not parent.mapRegion[x].has_key(y):
                parent.mapRegion[x][y] = {}
            for sprite in self.__Sprites:
                if sprite.x == tile.x and sprite.y == tile.y:
                    if sprite == self.__brush.sprites[-1]:
                        continue
                    if self.__brush.tile < 0 or (sprite != tile and sprite.z == tile.z):
                        self.__Sprites.remove(sprite)
            if self.__brush.tile < 0:
                del parent.mapRegion[x][y]
                continue
            depth = 0
            parent.mapRegion[x][y]['tile'] =  self.__brush.tile
            self.__Sprites.append(Sprite(QtCore.QRect(tile.rect),
                                    QtGui.QPixmap(tile.image), depth))
        self.__brush.sprites = self.__brush.sprites[-1:]
        self.update()

    def setBrush(self, tile):
        if not tile:
            if self.__brush:
                self.__Sprites.extend(self.__brush.sprites)
            else:
                return
        elif type(tile) is not Tile:
            raise TypeError('Brush tile must be instance of Tile class.')
        else:
            self.__brush = Tile({'tile': tile.id, 'data': tile, 'sprites': []})
            self.__brush.sprites.append(
                self.CreateNewSprite(-2147483615, 2147483615, 1, tile)
            )
            try:
                for sprite in self.__brush.sprites:
                    sprite.z = 1
            except Exception, e:
                print e
                return

    def removeBrush(self):
        self.clearBrush()
        self.__brush = None

    def clearBrush(self):
        if not self.__brush:
            return
        for sprite in self.__brush.sprites:
            self.__Sprites.remove(sprite)
        del self.__brush.sprites[:]

    def setBrushSize(self, size):
        if not self.__brush:
            return
        try:
            self.__brush.size = int(size)
        except Exception, e:
            print e
            return

    def moveBrush(self):
        brush = self.__brush
        if not brush:
            return
        x, y = fromMapPosition(self.__position['x'], self.__position['y'])
        if self.__draw:
            z = 1
            if brush.rectangle:
                minx, maxx = sorted([self.__position['x'], brush.rectangle['x']])
                miny, maxy = sorted([self.__position['y'], brush.rectangle['y']])
                for mx in range(minx, maxx+1):
                    for my in range(miny, maxy+1):
                        x, y = fromMapPosition(mx, my)
                        if len(filter(lambda sprite: sprite.x == x and sprite.y == y, brush.sprites)) < 1:
                            brush.sprites.append(self.CreateNewSprite(x, y, z, brush.data))

                def _r(sp):
                   sx, sy = toMapPosition(sp.x, sp.y)
                   if not minx <= sx <= maxx or not miny <= sy <= maxy:
                       self.__Sprites.remove(sp)
                       return False
                   return True
                brush.sprites = filter(_r, brush.sprites)
            else:
                if len(filter(lambda sprite: sprite.x == x and sprite.y == y, brush.sprites)) < 1:
                    brush.sprites.append(self.CreateNewSprite(x, y, z, brush.data))
        else:
            for sprite in brush.sprites:
                sprite.rect.moveTo(x, y)
        self.update()

    def moveCamera(self, x=0, y=0):
        self.__translate['x'] = x
        self.__translate['y'] = y

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
        self.connect(self.__Parent.ui.BrushSize.children()[-1],
                        QtCore.SIGNAL("valueChanged(int)"), self.setBrushSize)
        self.connect(self.__Parent.ui.MapErase, QtCore.SIGNAL("clicked()"), self.ChangeBrush)

    def keyPressEvent(self, event):
        self.__widget.keyPressEvent(event)

    def keyReleaseEvent(self, event):
        self.__widget.keyReleaseEvent(event)

    def LoadRegion(self, region):
        self.hide()
        self.__widget.moveCamera()
        self.ReloadObjects()
        self.mapRegion.clear()
        self.__widget.ClearSprites()
        if not region:
            return
        self.__regionName = region['name']
        if not self.__Tiles:
            return
        for tile in region.get('tiles', []):
            t = Tile(tile)
            if not t.tile:
                print "Bida! Tile at %s:%s has no tile id!" % (t.x, t.x)
                continue
            if t.tile not in self.__Tiles:
                print "Bida! Tile at %s:%s has non-declared id!" % (t.x, t.x)
                continue
            x, y = t.x, t.y
            if not self.mapRegion.has_key(x):
                self.mapRegion[x] = {}
            if not self.mapRegion[x].has_key(y):
                self.mapRegion[x][y] = {}
            self.mapRegion[x][y]['tile'] = t.tile
            tiledata = self.__Tiles[t.tile]
            mx, my = fromMapPosition(x, y)
            self.__widget.CreateNewSprite(mx, my, 0, tiledata)
        self.show()

    def ReloadObjects(self):
        files = fileManager.getFilesList(config.general.get('configs_path'), 'tiles')
        self.__Tiles.clear()
        row, col, maxcol = 0, 0, int(self.__Parent.ui.MapTilesPage.width()/64) - 2
        for f in files:
            data = lua.loadFile(os.path.join(config.path, config.general.get('configs_path'), str(f)))
            for tile in data:
                if type(tile) == dict and tile.has_key('id') and tile.has_key('image'):
                    self.__Tiles[tile['id']] = Tile(tile)
        for tile in self.__Tiles.values():
            image = sprites.createPixmap(tile.image, tile.picture)
            if not image:
                continue
            l = QtGui.QPushButton(self.__Parent.ui.MapTilesPage)
            tile.button = l
            l.setFlat(1)
            l.setCheckable(1)
            l.setIconSize(QtCore.QSize(image.width(), image.height()))
            l.setIcon(QtGui.QIcon(image))
            l.setObjectName(_fromUtf8('Tile'+str(tile.id)))
            self.connect(l, QtCore.SIGNAL("clicked()"), self.ChangeBrush)
            self.__Parent.ui.MapTilesPage.children()[0].addWidget(l, row, col)
            col += 1
            if col > maxcol:
                row += 1
                col = 0
        self.__Tiles['eraser'] = Tile(
                {'id': -1, 'image': 'ground.png', 'offsetx': 192,
                 'button': self.__Parent.ui.MapErase}
        )

    def ChangeBrush(self):
        button = self.sender()
        if button != self.__Parent.ui.MapErase:
            self.__Parent.ui.MapErase.setChecked(0)
        for child in self.__Parent.ui.MapTilesPage.children()[1:]:
            if child != button: child.setChecked(0)
        if not button.isChecked():
            button = None
        self.__widget.removeBrush()
        self.__BrushButton = button
        if not button:
            return
        for tile in self.__Tiles.values():
            if tile.button and tile.button == button:
                self.__widget.setBrush(tile)
                return

    def setBrushSize(self):
        self.__widget.setBrushSize(GetField(self.sender().parent()))

    #TODO: it's dirty
    def dump(self):
        self.hide()
        ret = {}
        ret['name'] = self.__regionName
        ret['tiles'] = []
        for i in self.mapRegion:
            for j in self.mapRegion[i]:
                try:
                    t = self.mapRegion[i][j]['tile']
                except:
                    continue
                if not t in self.__Tiles:
                    print 'Tile with id %s does not not exists' % t
                    continue
                if t != -1:
                    tile = {'x': i, 'y': j, 'tile': t}
                    ret['tiles'].append(tile)
        return ret
