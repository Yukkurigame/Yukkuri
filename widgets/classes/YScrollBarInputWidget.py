
from PyQt4 import QtCore

class YScrollBarInputWidget(object):

    def __init__(self, parent):
        self.connect(self.ui.scrollBar, QtCore.SIGNAL('valueChanged(int)'),
                     self, QtCore.SIGNAL('valueChanged(int)'))

    def minimum(self):
        return self.ui.scrollBar.minimum()

    def maximum(self):
        return self.ui.scrollBar.maximum()

    def diff(self):
        return self.maximum() - self.minimum()

    def setRange(self, minimum, maximum):
        bar = self.ui.scrollBar
        try:
            minimum = int(minimum)
        except:
            minimum = 0
        try:
            maximum = int(maximum)
        except:
            maximum = 0
        bar.setRange(minimum, maximum)
        bar.setValue(minimum)
