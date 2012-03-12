
import os.path

from PyQt4 import QtCore, QtGui
from config import config

class YImageViewWidget(QtGui.QGraphicsView):

    def __init__(self, parent=None):
        self._scene = QtGui.QGraphicsScene(self)
        QtGui.QGraphicsView.__init__(self, self._scene, parent)
        self.area = QtCore.QRectF(0, 0, 0, 0)
        self.areapen = QtGui.QPen(QtGui.QColor(255, 0, 0))
        self.areaItem = None
        self._mousePressed = False
        self.vLines = []
        self.hLines = []
        self.cols = 0
        self.rows = 0
        if parent:
            parent.wheelEvent = self.wheelEvent
            parent.mousePressEvent = self.mousePressEvent
            self._scene.mousePressEvent = self.mousePressEvent

    @QtCore.pyqtSlot(QtCore.QString)
    def setImage(self, filename):
        filename = os.path.join(config.path,
            config.general.get('images_path'), str(filename))
        image = QtGui.QPixmap(filename)
        self.setPixmap(image)

    @QtCore.pyqtSlot(QtGui.QPixmap)
    def setPixmap(self, image):
        self.clear()
        w, h = image.width(), image.height()
        self._scene.setSceneRect(0, 0, w, h)
        self._scene.addPixmap(image)
        self.areaItem = None
        self.vLines = self.hLines = []
        self._changeAreaSize()

    def clear(self):
        self._scene.clear()

    @QtCore.pyqtSlot(int)
    @QtCore.pyqtSlot(float)
    def setAreaWidth(self, width):
        self.area.setWidth(width)
        self._changeAreaSize()

    @QtCore.pyqtSlot(int)
    @QtCore.pyqtSlot(float)
    def setAreaHeight(self, height):
        self.area.setHeight(height)
        self._changeAreaSize()

    @QtCore.pyqtSlot(int)
    @QtCore.pyqtSlot(float)
    def setX(self, x):
        w = self.area.width()
        self.area.setX(x)
        self.setAreaWidth(w)

    @QtCore.pyqtSlot(int)
    @QtCore.pyqtSlot(float)
    def setY(self, y):
        h = self.area.height()
        self.area.setY(y)
        self.setAreaHeight(h)

    @QtCore.pyqtSlot(int)
    def setRows(self, rows):
        for line in self.hLines:
            self._scene.removeItem(line)
        self.hLines = []
        if not rows:
            rows = 1
        interval = int(self.area.height() / rows)
        if not interval:
            return
        for i in range(interval, rows * interval, interval):
            self.hLines.append(self._scene.addLine(self.area.x(),
            self.area.y() + i, self.area.x() + self.area.width(),
            self.area.y() + i, self.areapen))
        self.rows = rows

    @QtCore.pyqtSlot(int)
    def setColumns(self, cols):
        for line in self.vLines:
            self._scene.removeItem(line)
        self.vLines = []
        if not cols:
            cols = 1
        interval = int(self.area.width() / cols)
        if not interval:
            return
        for i in range(interval, cols * interval, interval):
            self.vLines.append(self._scene.addLine(self.area.x() + i,
            self.area.y(), self.area.x() + i,
            self.area.y() + self.area.height(), self.areapen))
        self.cols = cols

    def setScrollPolicy(self, v, h):
        self.setVerticalScrollBarPolicy(v)
        self.setHorizontalScrollBarPolicy(h)

    def _changeAreaSize(self):
        if self.areaItem:
            self._scene.removeItem(self.areaItem)
        self.areaItem = self._scene.addRect(self.area.x(),
            self.area.y(), self.area.width(),
            self.area.height(), self.areapen)
        self.setColumns(self.cols)
        self.setRows(self.rows)

    def wheelEvent(self, event):
        numDegrees = event.delta() / 8;
        numSteps = numDegrees / 15;

        if event.orientation() == QtCore.Qt.Horizontal or \
            QtGui.QApplication.keyboardModifiers() & QtCore.Qt.ControlModifier:
            scrollbar = self.horizontalScrollBar()
        else:
            scrollbar = self.verticalScrollBar()
        scrollbar.setValue(scrollbar.value() - scrollbar.singleStep() * numSteps)

    def mousePressEvent(self, event):
        self._mousePressed = True
        x, y = self.getEventScrollPoint(event)
        w, h = self.area.width(), self.area.height()
        self.area.setRect(x, y, w, h)
        self.emit(QtCore.SIGNAL("xChanged(int)"), x)
        self.emit(QtCore.SIGNAL("yChanged(int)"), y)

    def mouseReleaseEvent(self, event):
        self._mousePressed = False

    def mouseMoveEvent(self, event):
        if self._mousePressed:
            x, y = self.area.x(), self.area.y()
            w, h = self.getEventScrollPoint(event)
            w -= x
            h -= y
            if w or h:
                self.area.setRect(x, y, w, h)
                self.emit(QtCore.SIGNAL("widthChanged(int)"), w)
                self.emit(QtCore.SIGNAL("heightChanged(int)"), h)

    def getEventScrollPoint(self, event):
        hs = self.horizontalScrollBar()
        hs = (float(hs.value()) / float((hs.maximum() - hs.minimum()))
            ) * (self._scene.width() - self.width()) if hs.maximum() else 0
        vs = self.verticalScrollBar()
        vs = (float(vs.value()) / float((vs.maximum() - vs.minimum()))
            ) * (self._scene.height() - self.height()) if vs.maximum() else 0
        return (event.pos().x() + hs, event.pos().y() + vs)

