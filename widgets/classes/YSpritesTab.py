# -*- coding: utf-8 -*-

import os

from PyQt4 import QtCore, QtGui

from widgets import YAbstractTab

from config import config
from framework import GetWidget
from sprites import manager as sprites


class YSpritesTab(YAbstractTab):

    BOXES = {
        'Sprite': ['SpritesMainBox',],
    }
    EXTENSION = 'sprites'

    def __init__(self, parent=None):
        YAbstractTab.__init__(self, parent)
        box = self._Forms[0]
        GetWidget(box, 'image').setBasePath(os.path.join(
                config.path, config.general.get('images_path', '')))


    def saveFile(self):
        elname = YAbstractTab.saveFile(self)
        sprites.unloadImages()
        return elname
