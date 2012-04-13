# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from sprites import manager as sprites



class YTilesTab(YAbstractTab):

    BOXES = {
        'Tile': ['TilesMainBox',],
    }
    EXTENSION = 'tiles'

    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        self.ui.TileImageViewer.setScrollPolicy(
            QtCore.Qt.ScrollBarAlwaysOff, QtCore.Qt.ScrollBarAlwaysOff)


    @QtCore.pyqtSlot()
    def loadTileImage(self):
        #FIXME: This event emits two times: one on image changed and
        #       one on picture data set
        from widgets import YPictureComboBoxWidget
        self.ui.TileImageViewer.clear()
        sender = self.sender()
        print sender
        if not isinstance(sender, YPictureComboBoxWidget):
            return
        imagename = self.ui.Image.getValue()
        picture = sender.getValue()
        print imagename, picture
        if not imagename or picture in ('', None):
            return
        image = sprites.createPixmap(imagename, picture)
        if not image:
            return
        self.ui.TileImageViewer.setPixmap(image)

    def clearFields(self):
        self.loadTileImage()
        YAbstractTab.clearFields(self)
