
from PyQt4 import QtCore, QtGui

class YDoubleSpinBoxInputWidget:

    def __init__(self, parent=None):
        self.connect(self.ui.MultiplerSpinBox, QtCore.SIGNAL('valueChanged(int)'),
                    self.ui.doubleSpinBox.setSingleStep)
