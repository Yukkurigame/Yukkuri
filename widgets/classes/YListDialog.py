

from PyQt4 import QtCore, QtGui
from widgets.ui.Ui_YListDialog import Ui_YListDialog


class YListDialog(QtGui.QDialog):

    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.ui = Ui_YListDialog()
        self.ui.setupUi(self)

    def clear(self):
        item = self.ui.listWidget.takeItem(0)
        while item:
            item = self.ui.listWidget.takeItem(0)

    def insertItems(self, items):
        for item in items:
            item = QtGui.QListWidgetItem(item)
            self.ui.listWidget.addItem(item)

    def getItem(self):
        if self.exec_():
            item = self.ui.listWidget.currentItem()
            if item:
                return item.text()

