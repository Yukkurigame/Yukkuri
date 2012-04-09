# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractImageTab

from sprites import manager as sprites



class YTilesTab(YAbstractImageTab):

    BOXES = {
        'Tile': ['TilesMainBox',],
    }
    EXTENSION = 'tiles'

    def __init__(self, parent=None):
        YAbstractImageTab.__init__(self, parent)
        self.ui.TileImageViewer.setScrollPolicy(
            QtCore.Qt.ScrollBarAlwaysOff, QtCore.Qt.ScrollBarAlwaysOff)


    @QtCore.pyqtSlot()
    def loadTileImage(self):
        self.ui.TileImageViewer.clear()
        if not self.image or self.picture is None:
            return
        image = sprites.createPixmap(self.image.get('id'), self.picture)
        if not image:
            return
        self.ui.TileImageViewer.setPixmap(image)


    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item=None):
        YAbstractImageTab.reloadContent(self, item)
        self.loadTileImage()


    def clearFields(self):
        self.loadTileImage()
        YAbstractImageTab.clearFields(self)
