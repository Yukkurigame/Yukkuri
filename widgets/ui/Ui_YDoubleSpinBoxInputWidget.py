# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YDoubleSpinBoxInputWidget.ui'
#
# Created: Thu Sep 29 20:17:56 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YDoubleSpinBoxInputWidget(object):
    def setupUi(self, YDoubleSpinBoxInputWidget):
        YDoubleSpinBoxInputWidget.setObjectName(_fromUtf8("YDoubleSpinBoxInputWidget"))
        YDoubleSpinBoxInputWidget.resize(333, 23)
        YDoubleSpinBoxInputWidget.setWindowTitle(QtGui.QApplication.translate("YDoubleSpinBoxInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YDoubleSpinBoxInputWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YDoubleSpinBoxInputWidget)
        self.label.setText(QtGui.QApplication.translate("YDoubleSpinBoxInputWidget", "Text", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.doubleSpinBox_11 = QtGui.QDoubleSpinBox(YDoubleSpinBoxInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.doubleSpinBox_11.sizePolicy().hasHeightForWidth())
        self.doubleSpinBox_11.setSizePolicy(sizePolicy)
        self.doubleSpinBox_11.setMinimumSize(QtCore.QSize(120, 0))
        self.doubleSpinBox_11.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.doubleSpinBox_11.setObjectName(_fromUtf8("doubleSpinBox_11"))
        self.horizontalLayout.addWidget(self.doubleSpinBox_11)
        self.Multipler = QtGui.QWidget(YDoubleSpinBoxInputWidget)
        self.Multipler.setMaximumSize(QtCore.QSize(16777215, 20))
        self.Multipler.setObjectName(_fromUtf8("Multipler"))
        self.horizontalLayout_8 = QtGui.QHBoxLayout(self.Multipler)
        self.horizontalLayout_8.setMargin(0)
        self.horizontalLayout_8.setMargin(0)
        self.horizontalLayout_8.setObjectName(_fromUtf8("horizontalLayout_8"))
        self.label_2 = QtGui.QLabel(self.Multipler)
        self.label_2.setText(QtGui.QApplication.translate("YDoubleSpinBoxInputWidget", "X", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout_8.addWidget(self.label_2)
        self.spinBox_2 = QtGui.QSpinBox(self.Multipler)
        self.spinBox_2.setMaximumSize(QtCore.QSize(48, 16777215))
        self.spinBox_2.setMinimum(1)
        self.spinBox_2.setMaximum(999)
        self.spinBox_2.setProperty("value", 1)
        self.spinBox_2.setObjectName(_fromUtf8("spinBox_2"))
        self.horizontalLayout_8.addWidget(self.spinBox_2)
        self.horizontalLayout.addWidget(self.Multipler)

        self.retranslateUi(YDoubleSpinBoxInputWidget)
        QtCore.QMetaObject.connectSlotsByName(YDoubleSpinBoxInputWidget)

    def retranslateUi(self, YDoubleSpinBoxInputWidget):
        pass

