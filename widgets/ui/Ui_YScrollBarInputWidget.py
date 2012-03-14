# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YScrollBarInputWidget.ui'
#
# Created: Wed Mar 14 23:35:44 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YScrollBarInputWidget(object):
    def setupUi(self, YScrollBarInputWidget):
        YScrollBarInputWidget.setObjectName(_fromUtf8("YScrollBarInputWidget"))
        YScrollBarInputWidget.resize(239, 38)
        self.horizontalLayout = QtGui.QHBoxLayout(YScrollBarInputWidget)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.scrollBar = QtGui.QScrollBar(YScrollBarInputWidget)
        self.scrollBar.setEnabled(True)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.scrollBar.sizePolicy().hasHeightForWidth())
        self.scrollBar.setSizePolicy(sizePolicy)
        self.scrollBar.setMinimum(0)
        self.scrollBar.setMaximum(99)
        self.scrollBar.setOrientation(QtCore.Qt.Horizontal)
        self.scrollBar.setInvertedAppearance(False)
        self.scrollBar.setObjectName(_fromUtf8("scrollBar"))
        self.horizontalLayout.addWidget(self.scrollBar)
        self.spinBox = QtGui.QSpinBox(YScrollBarInputWidget)
        self.spinBox.setMaximumSize(QtCore.QSize(30, 16777215))
        self.spinBox.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.spinBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.spinBox.setMinimum(0)
        self.spinBox.setMaximum(99)
        self.spinBox.setProperty("value", 0)
        self.spinBox.setObjectName(_fromUtf8("spinBox"))
        self.horizontalLayout.addWidget(self.spinBox)

        self.retranslateUi(YScrollBarInputWidget)
        QtCore.QObject.connect(self.scrollBar, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), self.spinBox.setValue)
        QtCore.QObject.connect(self.spinBox, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), self.scrollBar.setValue)
        QtCore.QMetaObject.connectSlotsByName(YScrollBarInputWidget)

    def retranslateUi(self, YScrollBarInputWidget):
        YScrollBarInputWidget.setWindowTitle(QtGui.QApplication.translate("YScrollBarInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))

