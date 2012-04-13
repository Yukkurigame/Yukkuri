# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YComboBoxWidget
from sprites import manager as sprites



class YPictureComboBoxWidget(YComboBoxWidget):

    @QtCore.pyqtSlot('QString')
    def loadPictures(self, name):
        image = sprites.getImageById(name)
        if not image:
            return
        rows = image.get('rows', 1)
        cols = image.get('columns', 1)
        self.clear()
        for i in range(0, rows*cols):
            self.addItem(str(i))
