# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YComboBoxWidget(object):

    def __init__(self, *args, **kwargs):
        self.connect(self.ui.comboBox, QtCore.SIGNAL('currentIndexChanged(int)'), self, QtCore.SIGNAL('currentIndexChanged(int)'))

    def addItem(self, text):
        box = filter(lambda x: type(x) == QtGui.QComboBox,
                            self.children())[0]
        box.addItem(text)
