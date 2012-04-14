# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

class YMinusSpinBoxInputWidget(object):

    def __init__(self, parent=None):
        self.connect(self.ui.MultiplerSpinBox, QtCore.SIGNAL('valueChanged(int)'),
                    self.ui.spinBox.setSingleStep)
