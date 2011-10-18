# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Ui_YImageViewWidget.ui'
#
# Created: Tue Oct 18 16:05:55 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_YImageViewWidget(object):
    def setupUi(self, YImageViewWidget):
        YImageViewWidget.setObjectName(_fromUtf8("YImageViewWidget"))
        YImageViewWidget.resize(597, 304)
        YImageViewWidget.setAcceptDrops(True)
        YImageViewWidget.setWindowTitle(QtGui.QApplication.translate("YImageViewWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        YImageViewWidget.setStyleSheet(_fromUtf8("background: transparent;"))
        self.horizontalLayout = QtGui.QHBoxLayout(YImageViewWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.graphicsView = QtGui.QGraphicsView(YImageViewWidget)
        self.graphicsView.setStyleSheet(_fromUtf8("background: transparent;"))
        self.graphicsView.setFrameShape(QtGui.QFrame.NoFrame)
        self.graphicsView.setFrameShadow(QtGui.QFrame.Plain)
        brush = QtGui.QBrush(QtGui.QColor(0, 0, 0))
        brush.setStyle(QtCore.Qt.NoBrush)
        self.graphicsView.setBackgroundBrush(brush)
        brush = QtGui.QBrush(QtGui.QColor(0, 0, 0))
        brush.setStyle(QtCore.Qt.NoBrush)
        self.graphicsView.setForegroundBrush(brush)
        self.graphicsView.setInteractive(True)
        self.graphicsView.setDragMode(QtGui.QGraphicsView.NoDrag)
        self.graphicsView.setObjectName(_fromUtf8("graphicsView"))
        self.horizontalLayout.addWidget(self.graphicsView)

        self.retranslateUi(YImageViewWidget)
        QtCore.QMetaObject.connectSlotsByName(YImageViewWidget)

    def retranslateUi(self, YImageViewWidget):
        pass

