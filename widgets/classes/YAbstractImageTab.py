# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from framework import GetField, GetWidget
from sprites import manager as sprites


class YAbstractImageTab(YAbstractTab):

    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        from widgets import YImageChooser, YComboBoxWidget
        self.pictureBox = None
        self.image = None
        self.picture = None
        for form in self._Forms:
            img = GetWidget(form, 'image')
            if img and isinstance(img, YImageChooser):
                self.connect(img, QtCore.SIGNAL('valueChanged(QString)'), self.setImage)
            pic = GetWidget(form, 'picture')
            if pic:
                self.connect(pic, QtCore.SIGNAL('currentIndexChanged(int)'), self.setPicture)
                self.pictureBox = pic

    @QtCore.pyqtSlot('QString')
    def setImage(self, picname):
        self.image = image = sprites.getImageById(picname)
        if not image:
            return
        rows = image.get('rows', 1)
        cols = image.get('columns', 1)
        box = self.pictureBox
        box.clear()
        for i in range(0, rows*cols):
            box.addItem(str(i))

    @QtCore.pyqtSlot('int')
    def setPicture(self, index):
        try:
            self.picture = int(self.pictureBox.itemText(index))
        except:
            self.picture = None
