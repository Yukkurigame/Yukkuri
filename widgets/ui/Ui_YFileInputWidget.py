# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YFileInputWidget.ui'
#
# Created: Thu Sep 29 23:08:00 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YFileInputWidget(object):
    def setupUi(self, YFileInputWidget):
        YFileInputWidget.setObjectName(_fromUtf8("YFileInputWidget"))
        YFileInputWidget.resize(333, 24)
        YFileInputWidget.setWindowTitle(QtGui.QApplication.translate("YFileInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YFileInputWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YFileInputWidget)
        self.label.setText(QtGui.QApplication.translate("YFileInputWidget", "File:", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.lineEdit = QtGui.QLineEdit(YFileInputWidget)
        self.lineEdit.setMinimumSize(QtCore.QSize(0, 0))
        self.lineEdit.setMaximumSize(QtCore.QSize(150, 16777215))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout.addWidget(self.lineEdit)
        self.dialogButtonBox = QtGui.QDialogButtonBox(YFileInputWidget)
        self.dialogButtonBox.setMaximumSize(QtCore.QSize(75, 16777215))
        self.dialogButtonBox.setStandardButtons(QtGui.QDialogButtonBox.Open)
        self.dialogButtonBox.setObjectName(_fromUtf8("dialogButtonBox"))
        self.horizontalLayout.addWidget(self.dialogButtonBox)

        self.retranslateUi(YFileInputWidget)
        QtCore.QMetaObject.connectSlotsByName(YFileInputWidget)

    def retranslateUi(self, YFileInputWidget):
        pass

