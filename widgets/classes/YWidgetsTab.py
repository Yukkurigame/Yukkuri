# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

class YWidgetsTab(YAbstractTab):

    BOXES = {
        'Widget':       ['WidgetMainBox'],
        'TextWidget':   ['WidgetMainBox', 'WidgetTextBox'],
        'BarWidget':    ['WidgetMainBox', 'WidgetTextBox'],
    }
    EXTENSION = 'widget'
