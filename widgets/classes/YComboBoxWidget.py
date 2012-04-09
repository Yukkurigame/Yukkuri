# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YComboBoxWidget(object):

    def __init__(self, *args, **kwargs):
        self.connect(self.ui.comboBox, QtCore.SIGNAL('currentIndexChanged(int)'), self, QtCore.SIGNAL('currentIndexChanged(int)'))

    def __getBox(self):
        if not getattr(self,  'box',  None):
            self.__box = filter(lambda x: type(x) == QtGui.QComboBox,
                            self.children())[0]
        return self.__box

    def addItem(self, text):
        self.__getBox().addItem(text)

    def itemText(self, index):
        return self.__getBox().itemText(index)

    def clear(self):
        self.__getBox().clear()
