# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YColorInputWidget.ui'
#
# Created: Fri Mar  9 15:27:42 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YColorInputWidget(object):
    def setupUi(self, YColorInputWidget):
        YColorInputWidget.setObjectName(_fromUtf8("YColorInputWidget"))
        YColorInputWidget.resize(333, 23)
        self.horizontalLayout = QtGui.QHBoxLayout(YColorInputWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YColorInputWidget)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.spinBox = QtGui.QSpinBox(YColorInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox.sizePolicy().hasHeightForWidth())
        self.spinBox.setSizePolicy(sizePolicy)
        self.spinBox.setMaximumSize(QtCore.QSize(38, 16777215))
        self.spinBox.setFrame(True)
        self.spinBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.spinBox.setKeyboardTracking(False)
        self.spinBox.setMaximum(256)
        self.spinBox.setObjectName(_fromUtf8("spinBox"))
        self.horizontalLayout.addWidget(self.spinBox)
        self.spinBox_2 = QtGui.QSpinBox(YColorInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_2.sizePolicy().hasHeightForWidth())
        self.spinBox_2.setSizePolicy(sizePolicy)
        self.spinBox_2.setMaximumSize(QtCore.QSize(38, 16777215))
        self.spinBox_2.setFrame(True)
        self.spinBox_2.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.spinBox_2.setKeyboardTracking(False)
        self.spinBox_2.setMaximum(256)
        self.spinBox_2.setObjectName(_fromUtf8("spinBox_2"))
        self.horizontalLayout.addWidget(self.spinBox_2)
        self.spinBox_1 = QtGui.QSpinBox(YColorInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_1.sizePolicy().hasHeightForWidth())
        self.spinBox_1.setSizePolicy(sizePolicy)
        self.spinBox_1.setMaximumSize(QtCore.QSize(38, 16777215))
        self.spinBox_1.setFrame(True)
        self.spinBox_1.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.spinBox_1.setKeyboardTracking(False)
        self.spinBox_1.setMaximum(256)
        self.spinBox_1.setObjectName(_fromUtf8("spinBox_1"))
        self.horizontalLayout.addWidget(self.spinBox_1)
        self.toolButton = QtGui.QToolButton(YColorInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.toolButton.sizePolicy().hasHeightForWidth())
        self.toolButton.setSizePolicy(sizePolicy)
        self.toolButton.setObjectName(_fromUtf8("toolButton"))
        self.horizontalLayout.addWidget(self.toolButton)

        self.retranslateUi(YColorInputWidget)
        QtCore.QMetaObject.connectSlotsByName(YColorInputWidget)

    def retranslateUi(self, YColorInputWidget):
        YColorInputWidget.setWindowTitle(QtGui.QApplication.translate("YColorInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("YColorInputWidget", "Blood color:", None, QtGui.QApplication.UnicodeUTF8))
        self.toolButton.setText(QtGui.QApplication.translate("YColorInputWidget", "Choose", None, QtGui.QApplication.UnicodeUTF8))

