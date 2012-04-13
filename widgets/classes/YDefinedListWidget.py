# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from YListDialog import YListDialog


class YDefinedListWidget(QtGui.QListWidget):

    def __init__(self, parent=None):
        QtGui.QListWidget.__init__(self, parent)
        self.cdialog = YListDialog(self)
        self.customContextMenuRequested.connect(self.listContextMenu)

    def setItems(self, data):
        self.cdialog.clear()
        self.cdialog.insertItems(sorted(data))

    def listContextMenu(self, pos):
        menu = QtGui.QMenu()
        item = self.itemAt(pos)
        #This shit sends without event
        a = menu.addAction("Add item")
        #oh, lol
        self.connect(a, QtCore.SIGNAL('triggered()'),
                        lambda r=item: self.chooseItem(r))
        menu.addAction(a)
        if item:
            d = menu.addAction("Delete item")
            self.connect(d, QtCore.SIGNAL('triggered()'),
                        lambda r=item: self.removeItem(r))
            menu.addAction(d)
        menu.exec_(self.mapToGlobal(pos))

    def removeItem(self, item):
        #lol
        self.takeItem(self.row(item))

    def chooseItem(self, aitem=None):
        name = self.cdialog.getItem()
        if not name:
            return
        item = QtGui.QListWidgetItem(name)
        row = self.row(aitem)
        if row < 0:
            self.addItem(item)
        else:
            self.insertItem(row, item)

    def getValue(self):
        data = []
        for i in range(self.count()):
            data.append(str(self.item(i).text()))
        return data

    def setValue(self, data):
        self.clear()
        if type(data) in (list, tuple):
            for name in data:
                self.addItem(QtGui.QListWidgetItem(name))
        self._changed(data)

    def _changed(self, *data):
        types = ', '.join(map(lambda x: type(x).__name__, data))
        self.emit(QtCore.SIGNAL("valueChanged(%s)" % types), *data)
