# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YSpinBoxInputWidget.ui'
#
# Created: Thu Oct 20 21:17:04 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YSpinBoxInputWidget(object):
    def setupUi(self, YSpinBoxInputWidget):
        YSpinBoxInputWidget.setObjectName(_fromUtf8("YSpinBoxInputWidget"))
        YSpinBoxInputWidget.resize(330, 23)
        YSpinBoxInputWidget.setWindowTitle(QtGui.QApplication.translate("YSpinBoxInputWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YSpinBoxInputWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YSpinBoxInputWidget)
        self.label.setText(QtGui.QApplication.translate("YSpinBoxInputWidget", "Text", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.spinBox = QtGui.QSpinBox(YSpinBoxInputWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox.sizePolicy().hasHeightForWidth())
        self.spinBox.setSizePolicy(sizePolicy)
        self.spinBox.setMinimumSize(QtCore.QSize(141, 0))
        self.spinBox.setMaximumSize(QtCore.QSize(141, 16777215))
        self.spinBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox.setMinimum(0)
        self.spinBox.setMaximum(999999999)
        self.spinBox.setProperty("value", 0)
        self.spinBox.setObjectName(_fromUtf8("spinBox"))
        self.horizontalLayout.addWidget(self.spinBox)
        self.Multipler = QtGui.QWidget(YSpinBoxInputWidget)
        self.Multipler.setMaximumSize(QtCore.QSize(16777215, 20))
        self.Multipler.setObjectName(_fromUtf8("Multipler"))
        self.horizontalLayout_8 = QtGui.QHBoxLayout(self.Multipler)
        self.horizontalLayout_8.setMargin(0)
        self.horizontalLayout_8.setMargin(0)
        self.horizontalLayout_8.setObjectName(_fromUtf8("horizontalLayout_8"))
        self.label_2 = QtGui.QLabel(self.Multipler)
        self.label_2.setText(QtGui.QApplication.translate("YSpinBoxInputWidget", "X", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout_8.addWidget(self.label_2)
        self.MultiplerSpinBox = QtGui.QSpinBox(self.Multipler)
        self.MultiplerSpinBox.setMaximumSize(QtCore.QSize(48, 16777215))
        self.MultiplerSpinBox.setMinimum(1)
        self.MultiplerSpinBox.setMaximum(999)
        self.MultiplerSpinBox.setProperty("value", 1)
        self.MultiplerSpinBox.setObjectName(_fromUtf8("MultiplerSpinBox"))
        self.horizontalLayout_8.addWidget(self.MultiplerSpinBox)
        self.horizontalLayout.addWidget(self.Multipler)

        self.retranslateUi(YSpinBoxInputWidget)
        QtCore.QObject.connect(self.spinBox, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), YSpinBoxInputWidget._changed)
        QtCore.QMetaObject.connectSlotsByName(YSpinBoxInputWidget)

    def retranslateUi(self, YSpinBoxInputWidget):
        pass

