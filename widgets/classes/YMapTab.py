# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from map import MapWindow


class YMapTab(YAbstractTab):

    BOXES = {
        'region': ['MapOptionsBox',],
    }
    EXTENSION = 'map'

    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        self.__Map = MapWindow(self)

    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item=None):
        YAbstractTab.reloadContent(self, item)
        self.__Map.LoadRegion(self.getCurrentElement())

    def getSaved(self, eltype):
        return self.__Map.dump()
