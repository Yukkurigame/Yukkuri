
from PyQt4 import QtCore, QtGui

class YTableWidget(object):
    pass

    def __init__(self, parent=None):
        self._table = table = self.ui.tableWidget
        table.customContextMenuRequested.connect(self.tableContextMenu)

    def tableContextMenu(self, pos):
        menu = QtGui.QMenu()
        row = self._table.rowAt(pos.y())
        #This shit sends without event
        a = menu.addAction("New row")
        #oh, lol
        self.connect(a, QtCore.SIGNAL('triggered()'),
                        lambda r=row: self.tableAddRow(r))
        menu.addAction(a)
        if row >= 0:
            d = menu.addAction("Delete row")
            self.connect(d, QtCore.SIGNAL('triggered()'),
                        lambda r=row: self.tableRemoveRow(r))
            menu.addAction(d)
        menu.exec_(self.mapToGlobal(pos))

    @QtCore.pyqtSlot(int)
    def tableAddRow(self, row):
        self._table.insertRow(row + 1)

    @QtCore.pyqtSlot(int)
    def tableRemoveRow(self, row):
        if row < 0:
            return
        self._table.removeRow(row)

    def setHorizontalHeaderLabels(self, l):
        self._table.setColumnCount(len(l))
        self._table.setHorizontalHeaderLabels(l)

    def loadDataDict(self, data):
        table = self._table
        table.clearContents()
        row = 0
        for k, v in data.items():
            if table.rowCount() <= row:
                table.insertRow(row)
            table.setItem(row, 0, QtGui.QTableWidgetItem(k))
            col = 1
            for j in v:
                table.setItem(row, col, QtGui.QTableWidgetItem(str(j)))
                col += 1
            row += 1
