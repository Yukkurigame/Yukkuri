# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YFontComboBoxWidget(object):


    def getValue(self):
        font = self.ui.fontComboBox.currentFont()
        return font
