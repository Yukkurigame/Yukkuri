# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YTreeDialog.ui'
#
# Created: Thu Oct 20 18:45:04 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YTreeDialog(object):
    def setupUi(self, YTreeDialog):
        YTreeDialog.setObjectName(_fromUtf8("YTreeDialog"))
        YTreeDialog.resize(398, 292)
        YTreeDialog.setWindowTitle(QtGui.QApplication.translate("YTreeDialog", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = QtGui.QVBoxLayout(YTreeDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.treeWidget = QtGui.QTreeWidget(YTreeDialog)
        self.treeWidget.setEditTriggers(QtGui.QAbstractItemView.NoEditTriggers)
        self.treeWidget.setAlternatingRowColors(True)
        self.treeWidget.setObjectName(_fromUtf8("treeWidget"))
        self.treeWidget.headerItem().setText(0, QtGui.QApplication.translate("YTreeDialog", "Sprites", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout.addWidget(self.treeWidget)
        self.buttonBox = QtGui.QDialogButtonBox(YTreeDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(YTreeDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), YTreeDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), YTreeDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(YTreeDialog)

    def retranslateUi(self, YTreeDialog):
        pass

