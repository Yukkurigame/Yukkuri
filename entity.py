from PyQt4 import QtGui, QtCore
from framework import *
from math import floor

class EntityTab:

    def __init__(self, main):
        self.main = main
        main.ui.Animation.setHorizontalHeaderLabels(
            ['Animation name', 'Start', 'End'])
        main.ui.Animation.loadDataDict({
            'rightup': [0, 0],
            'right': [0, 0],
            'up': [0, 0],
            'down': [0, 0],
            'leftdown': [0, 0],
            'rightdown': [0, 0],
            'leftup': [0, 0],
            'left': [0, 0],
        })
        #lol
        main.ui.EntityAnimationSettings._imageName = lambda: \
            GetField(GetWidget(self.main.ui.EntityMainBox, 'image'))

    def setPerviewWidth(self):
        field = self.main.sender()
        width = field.value()
        widget = self.main.ui.EntityAnimationViewer
        widget.resize(width, widget.height())

    def setPerviewHeight(self):
        field = self.main.sender()
        height = field.value()
        self.main.ui.EntityAnimationViewer.resize(self.main.ui.EntityAnimationViewer.width(), height)

    def loadAnimation(self, data):
        self.main.ui.Animation.loadDataDict(data)
