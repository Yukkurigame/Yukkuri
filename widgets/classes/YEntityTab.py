# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from framework import GetField, GetWidget


class YEntityTab(YAbstractTab):

    BOXES = {
        'Entity':   ['EntityMainBox', 'EntityEntityBox', 'EntityMiscBox',
                     'EntityAnimationBox'],
        'Plant':    ['EntityMainBox', 'EntityMiscBox', 'EntityFeedBox'],
        'Corpse':   ['EntityMainBox', 'EntityFeedBox'],
    }
    EXTENSION = 'entity'


    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        self.ui.Animation.setHorizontalHeaderLabels(
            ['Animation name', 'Start', 'End'])
        self.ui.Animation.loadDataDict({
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
        self.ui.EntityAnimationSettings._imageName = lambda: \
            GetField(GetWidget(self.ui.EntityMainBox, 'image'))

    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item=None):
        YAbstractTab.reloadContent(self, item)
        element = self.getCurrentElement()
        if type(element) is dict:
            self.loadAnimation(element.get('animation'))

    def setPerviewWidth(self):
        field = self.main.sender()
        width = field.value()
        widget = self.ui.EntityAnimationViewer
        widget.resize(width, widget.height())

    def setPerviewHeight(self):
        field = self.main.sender()
        height = field.value()
        self.ui.EntityAnimationViewer.resize(self.ui.EntityAnimationViewer.width(), height)

    def loadAnimation(self, data):
        self.ui.Animation.loadDataDict(data or {})
