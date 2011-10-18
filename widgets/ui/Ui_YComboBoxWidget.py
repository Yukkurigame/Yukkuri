# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YComboBoxWidget.ui'
#
# Created: Wed Oct 12 20:13:43 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YComboBoxWidget(object):
    def setupUi(self, YComboBoxWidget):
        YComboBoxWidget.setObjectName(_fromUtf8("YComboBoxWidget"))
        YComboBoxWidget.resize(253, 24)
        YComboBoxWidget.setWindowTitle(QtGui.QApplication.translate("YComboBoxWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YComboBoxWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YComboBoxWidget)
        self.label.setText(QtGui.QApplication.translate("YComboBoxWidget", "Text", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.comboBox = QtGui.QComboBox(YComboBoxWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox.sizePolicy().hasHeightForWidth())
        self.comboBox.setSizePolicy(sizePolicy)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.horizontalLayout.addWidget(self.comboBox)

        self.retranslateUi(YComboBoxWidget)
        QtCore.QMetaObject.connectSlotsByName(YComboBoxWidget)

    def retranslateUi(self, YComboBoxWidget):
        pass

