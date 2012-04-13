# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YColorInputWidget(object):

    def __init__(self, parent=None):
        self.connect(self.ui.toolButton, QtCore.SIGNAL('clicked()'),
                    self.changeColor)

    def changeColor(self):
        color = self.getValue()
        try:
            color = map(lambda x: int(x), color)
            color = QtGui.QColor(*color)
        except:
            color = QtGui.QColor("linen")
        color = QtGui.QColorDialog.getColor(color, self, "Choose color")
        if not color.isValid():
            return
        self.setValue(color.getRgb()[:3])

    def setValue(self, *data):
        lines = filter(lambda x: type(x) is QtGui.QSpinBox, self.children())
        data = data[-1]
        for i in range(0, len(lines)):
            field = 0
            if i < len(data):
                field = data[i]
            try:
                lines[i].setValue(int(field))
            except:
                pass

