# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YSimpleInputWidget.ui'
#
# Created: Thu Sep 29 19:32:02 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YSimpleInputWidget(object):
    def setupUi(self, YSimpleInputWidget):
        YSimpleInputWidget.setObjectName(_fromUtf8("YSimpleInputWidget"))
        YSimpleInputWidget.resize(330, 23)
        YSimpleInputWidget.setWindowTitle(QtGui.QApplication.translate("YSimpleInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YSimpleInputWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YSimpleInputWidget)
        self.label.setText(QtGui.QApplication.translate("YSimpleInputWidget", "Text:", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(77, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.lineEdit = QtGui.QLineEdit(YSimpleInputWidget)
        self.lineEdit.setMinimumSize(QtCore.QSize(210, 0))
        self.lineEdit.setMaximumSize(QtCore.QSize(190, 16777215))
        self.lineEdit.setInputMethodHints(QtCore.Qt.ImhNone)
        self.lineEdit.setReadOnly(False)
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout.addWidget(self.lineEdit)

        self.retranslateUi(YSimpleInputWidget)
        QtCore.QMetaObject.connectSlotsByName(YSimpleInputWidget)

    def retranslateUi(self, YSimpleInputWidget):
        pass

