# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YTableWidget.ui'
#
# Created: Tue Mar 13 21:12:52 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YTableWidget(object):
    def setupUi(self, YTableWidget):
        YTableWidget.setObjectName(_fromUtf8("YTableWidget"))
        YTableWidget.resize(264, 241)
        self.horizontalLayout = QtGui.QHBoxLayout(YTableWidget)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.tableWidget = QtGui.QTableWidget(YTableWidget)
        self.tableWidget.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.tableWidget.setRowCount(0)
        self.tableWidget.setColumnCount(0)
        self.tableWidget.setObjectName(_fromUtf8("tableWidget"))
        self.tableWidget.horizontalHeader().setVisible(True)
        self.tableWidget.horizontalHeader().setCascadingSectionResizes(True)
        self.tableWidget.horizontalHeader().setDefaultSectionSize(120)
        self.tableWidget.horizontalHeader().setHighlightSections(True)
        self.tableWidget.horizontalHeader().setSortIndicatorShown(True)
        self.tableWidget.horizontalHeader().setStretchLastSection(True)
        self.tableWidget.verticalHeader().setVisible(False)
        self.tableWidget.verticalHeader().setSortIndicatorShown(False)
        self.tableWidget.verticalHeader().setStretchLastSection(False)
        self.horizontalLayout.addWidget(self.tableWidget)

        self.retranslateUi(YTableWidget)
        QtCore.QMetaObject.connectSlotsByName(YTableWidget)

    def retranslateUi(self, YTableWidget):
        YTableWidget.setWindowTitle(QtGui.QApplication.translate("YTableWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))

