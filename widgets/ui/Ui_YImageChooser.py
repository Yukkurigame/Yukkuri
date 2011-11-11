# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YImageChooser.ui'
#
# Created: Tue Nov  8 01:53:23 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YImageChooser(object):
    def setupUi(self, YImageChooser):
        YImageChooser.setObjectName(_fromUtf8("YImageChooser"))
        YImageChooser.resize(330, 24)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(YImageChooser.sizePolicy().hasHeightForWidth())
        YImageChooser.setSizePolicy(sizePolicy)
        YImageChooser.setWindowTitle(QtGui.QApplication.translate("YImageChooser", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalLayout = QtGui.QHBoxLayout(YImageChooser)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(YImageChooser)
        self.label.setText(QtGui.QApplication.translate("YImageChooser", "Image", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(22, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.EntityImgField = QtGui.QLineEdit(YImageChooser)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.EntityImgField.sizePolicy().hasHeightForWidth())
        self.EntityImgField.setSizePolicy(sizePolicy)
        self.EntityImgField.setMinimumSize(QtCore.QSize(129, 0))
        self.EntityImgField.setMaximumSize(QtCore.QSize(129, 16777215))
        self.EntityImgField.setReadOnly(True)
        self.EntityImgField.setObjectName(_fromUtf8("EntityImgField"))
        self.horizontalLayout.addWidget(self.EntityImgField)
        self.ImageChooseButton = QtGui.QPushButton(YImageChooser)
        self.ImageChooseButton.setText(QtGui.QApplication.translate("YImageChooser", "Choose", None, QtGui.QApplication.UnicodeUTF8))
        self.ImageChooseButton.setAutoDefault(False)
        self.ImageChooseButton.setDefault(False)
        self.ImageChooseButton.setFlat(False)
        self.ImageChooseButton.setObjectName(_fromUtf8("ImageChooseButton"))
        self.horizontalLayout.addWidget(self.ImageChooseButton)

        self.retranslateUi(YImageChooser)
        QtCore.QMetaObject.connectSlotsByName(YImageChooser)

    def retranslateUi(self, YImageChooser):
        pass

