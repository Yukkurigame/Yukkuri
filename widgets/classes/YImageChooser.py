# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YImageChooser:

    def __init__(self, parent=None):
        self.connect(self.ui.ImageChooseButton, QtCore.SIGNAL('clicked()'),
                    self.getImages)

    def getImages(self):
        pass
