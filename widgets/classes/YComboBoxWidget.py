# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YComboBoxWidget(object):

    def addItem(self, text):
        box = filter(lambda x: type(x) == QtGui.QComboBox,
                            self.children())[0]
        box.addItem(text)

