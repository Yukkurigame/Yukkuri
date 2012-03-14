from PyQt4 import QtGui, QtCore
from framework import *
from math import floor

class EntityTab:

    def __init__(self, main):
        self.main = main
        main.ui.EntityAnimationTable.setHorizontalHeaderLabels(
            ['Animation name', 'Start', 'End'])
        main.ui.EntityAnimationTable.loadDataDict({
            'rightup': [0, 1],
            'right': [0, 1],
            'up': [0, 1],
            'down': [0, 1],
            'leftdown': [0, 1],
            'rightdown': [0, 1],
            'leftup': [0, 1],
            'left': [0, 1],
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
        self.main.ui.EntityAnimationTable.loadDataDict(data)
