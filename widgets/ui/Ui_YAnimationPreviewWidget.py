# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YAnimationPreviewWidget.ui'
#
# Created: Thu Mar 15 22:45:31 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YAnimationPreviewWidget(object):
    def setupUi(self, YAnimationPreviewWidget):
        YAnimationPreviewWidget.setObjectName(_fromUtf8("YAnimationPreviewWidget"))
        YAnimationPreviewWidget.resize(313, 191)
        self.horizontalLayout = QtGui.QHBoxLayout(YAnimationPreviewWidget)
        self.horizontalLayout.setSpacing(6)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.View = QtGui.QFrame(YAnimationPreviewWidget)
        self.View.setMinimumSize(QtCore.QSize(64, 64))
        self.View.setMaximumSize(QtCore.QSize(200, 200))
        self.View.setFrameShape(QtGui.QFrame.Box)
        self.View.setFrameShadow(QtGui.QFrame.Sunken)
        self.View.setObjectName(_fromUtf8("View"))
        self.horizontalLayout_8 = QtGui.QHBoxLayout(self.View)
        self.horizontalLayout_8.setSpacing(0)
        self.horizontalLayout_8.setMargin(0)
        self.horizontalLayout_8.setObjectName(_fromUtf8("horizontalLayout_8"))
        self.AnimationViewer = YImageViewWidget(self.View)
        self.AnimationViewer.setToolTip(_fromUtf8(""))
        self.AnimationViewer.setStyleSheet(_fromUtf8("background: transparent;"))
        self.AnimationViewer.setObjectName(_fromUtf8("AnimationViewer"))
        self.horizontalLayout_8.addWidget(self.AnimationViewer)
        self.horizontalLayout.addWidget(self.View)
        self.Control = QtGui.QWidget(YAnimationPreviewWidget)
        self.Control.setObjectName(_fromUtf8("Control"))
        self.verticalLayout_5 = QtGui.QVBoxLayout(self.Control)
        self.verticalLayout_5.setMargin(0)
        self.verticalLayout_5.setObjectName(_fromUtf8("verticalLayout_5"))
        self.comboBox = QtGui.QComboBox(self.Control)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.verticalLayout_5.addWidget(self.comboBox)
        self.Frame = YScrollBarInputWidget(self.Control)
        self.Frame.setEnabled(True)
        self.Frame.setObjectName(_fromUtf8("Frame"))
        self.verticalLayout_5.addWidget(self.Frame)
        self.Delay = YSpinBoxInputWidget(self.Control)
        self.Delay.setObjectName(_fromUtf8("Delay"))
        self.verticalLayout_5.addWidget(self.Delay)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.pushButton = QtGui.QPushButton(self.Control)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButton.sizePolicy().hasHeightForWidth())
        self.pushButton.setSizePolicy(sizePolicy)
        self.pushButton.setAutoDefault(False)
        self.pushButton.setDefault(False)
        self.pushButton.setFlat(False)
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.horizontalLayout_2.addWidget(self.pushButton)
        self.Loop = QtGui.QCheckBox(self.Control)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.Loop.sizePolicy().hasHeightForWidth())
        self.Loop.setSizePolicy(sizePolicy)
        self.Loop.setMaximumSize(QtCore.QSize(52, 16777215))
        self.Loop.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.Loop.setObjectName(_fromUtf8("Loop"))
        self.horizontalLayout_2.addWidget(self.Loop)
        self.verticalLayout_5.addLayout(self.horizontalLayout_2)
        self.horizontalLayout.addWidget(self.Control)

        self.retranslateUi(YAnimationPreviewWidget)
        QtCore.QMetaObject.connectSlotsByName(YAnimationPreviewWidget)

    def retranslateUi(self, YAnimationPreviewWidget):
        YAnimationPreviewWidget.setWindowTitle(QtGui.QApplication.translate("YAnimationPreviewWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton.setText(QtGui.QApplication.translate("YAnimationPreviewWidget", "Play", None, QtGui.QApplication.UnicodeUTF8))
        self.Loop.setText(QtGui.QApplication.translate("YAnimationPreviewWidget", "Loop", None, QtGui.QApplication.UnicodeUTF8))

from widgets import YImageViewWidget, YScrollBarInputWidget, YSpinBoxInputWidget
