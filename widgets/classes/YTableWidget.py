
from PyQt4 import QtCore, QtGui

class YTableWidget(object):

    _ignoreChanges = False

    def __init__(self, parent=None):
        self._table = table = self.ui.tableWidget
        table.customContextMenuRequested.connect(self.tableContextMenu)
        table.cellChanged.connect(self.tableChanged)

    def getValue(self):
        return self.getDataDict()

    @QtCore.pyqtSlot(int)
    def tableAddRow(self, row):
        self._table.insertRow(row + 1)

    @QtCore.pyqtSlot(int)
    def tableRemoveRow(self, row):
        if row < 0:
            return
        self._table.removeRow(row)

    def tableChanged(self, row, column):
        if not self._ignoreChanges:
            self.emit(QtCore.SIGNAL("changed(PyQt_PyObject)"), self.getData())

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

    def setHorizontalHeaderLabels(self, l):
        self._table.setColumnCount(len(l))
        self._table.setHorizontalHeaderLabels(l)

    def loadDataDict(self, data):
        table = self._table
        table.clearContents()
        row = 0
        self._ignoreChanges = True
        for k, v in data.items():
            if table.rowCount() <= row:
                table.insertRow(row)
            table.setItem(row, 0, QtGui.QTableWidgetItem(k))
            col = 1
            for j in v:
                table.setItem(row, col, QtGui.QTableWidgetItem(str(j)))
                col += 1
            row += 1
        self._ignoreChanges = False
        self.tableChanged(0, 0)

    def getData(self):
        data = []
        table = self._table
        for row in range(0, table.rowCount()):
            cdata = []
            for column in range(0, table.columnCount()):
                item = table.item(row, column)
                cdata.append(item.text() if item else '')
            data.append(cdata)
        return data

    def getDataDict(self):
        data = {}
        table = self._table
        for row in range(0, table.rowCount()):
            key = table.item(row, 0)
            if key:
                key = str(key.text())
            if not key:
                continue
            cdata = []
            for column in range(1, table.columnCount()):
                item = table.item(row, column)
                cdata.append(int(item.text()) if item else 0)
            data[key] = cdata
        return data
