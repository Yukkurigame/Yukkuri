# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class YSpinBoxInputWidget(object):

    def __init__(self, parent=None):
        self.connect(self.ui.MultiplerSpinBox, QtCore.SIGNAL('valueChanged(int)'),
                    self.ui.spinBox.setSingleStep)
