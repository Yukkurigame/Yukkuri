# -*- coding: utf-8 -*-

import os

from PyQt4 import QtCore, QtGui

from config import config
from files import lua
from framework import RefillFields


class YGeneralConfigDialog(QtGui.QDialog):

    def __init__(self, parent=None):
        self._configData = None
        self.loaded = False
        self.load()

    def load(self):
        if self.loaded:
            return True
        if not config.path:
            return
        try:
            self._configData = lua.load(os.path.join(config.path,
                config.general.get('configs_path') or 'data/defs',
                'general.config'))[0]
            config.general = self._configData
            self.loaded = True
            return True
        except (IndexError, TypeError):
            print "Bad main config"

    def show(self):
        QtGui.QDialog.show(self)
        if not self._configData or not self.load():
                return
        for el in filter(lambda x: type(x) == QtGui.QGroupBox, self.children()):
            RefillFields(el, self._configData)

    def accept(self):
        self.save()
        QtGui.QDialog.accept(self)

    def save(self):
        saved = {}
        for box in filter(lambda x: type(x) == QtGui.QGroupBox, self.children()):
            for child in box.children()[1:]:
                try:
                    field = child.getValue()
                except Exception, e:
                    print str(e)
                    field = GetField(child)
                if field:
                    name = re.sub('_\d+$', '', str(child.objectName()).lower())
                    saved[name] = field
        saved.update({"name": 'general', "type": 'config', "id": 'config_general'})
        self._configData = config.general = saved
        saved = [saved,]
        lua.dump(os.path.join(config.path,
            config.general.get('configs_path') or 'data/defs',
            'general.config'), saved)
