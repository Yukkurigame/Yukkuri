# -*- coding: utf-8 -*-

import os

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from config import config
from framework import GetWidget, GetField
from gui import manager as wmanager


class YWidgetsTab(YAbstractTab):

    BOXES = {
        'Widget':       ['WidgetMainBox', 'WidgetChildrenBox'],
        'TextWidget':   ['WidgetMainBox', 'WidgetTextBox',
                         'WidgetChildrenBox'],
        'BarWidget':    ['WidgetMainBox', 'WidgetTextBox',
                         'WidgetBarBox', 'WidgetChildrenBox'],
    }
    EXTENSION = 'widget'

    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        GetWidget(self.ui.WidgetTextBox, 'Font').setBasePath(
            os.path.join(config.path, config.general.get('fonts_path', '')))
        align = GetWidget(self.ui.WidgetMainBox, 'Align')
        valign = GetWidget(self.ui.WidgetMainBox, 'VAlign')
        talign = GetWidget(self.ui.WidgetTextBox, 'TextAlign')
        for variant in ['Left', 'Center', 'Right']:
            align.addItem(variant)
            talign.addItem(variant)
        for variant in ['Top', 'Middle', 'Bottom']:
            valign.addItem(variant)

    def saveFile(self):
        elname = YAbstractTab.saveFile(self)
        wmanager.loadWidgets()
        return elname

    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item):
        self.ui.Children.clear()
        self.ui.Children.setItems(wmanager.getWidgetsById().keys())
        YAbstractTab.reloadContent(self, item)
