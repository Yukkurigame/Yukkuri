# -*- coding: utf-8 -*-

import os
import re

from PyQt4 import QtCore, QtGui

from random import randint

from files import lua, fileManager
from config import config
from framework import RefillFields, GetWidget, GetField


class YAbstractTab(object):

    BOXES = {}
    EXTENSION = ''


    def __init__(self, parent=None):
        self._loadedConfig = []
        self._loadedFile = ''
        self._loadedElement = ''
        self.setBase(parent)
        self._Forms = []
        self._BOXES = dict([(k.lower(), v) for k, v in self.BOXES.items()])
        boxes = []
        [boxes.extend(v) for v in self.BOXES.values()]
        boxes = set(boxes)
        for box in boxes:
            b = getattr(self.ui, box)
            if b not in self._Forms:
                self._Forms.append(b)
            w = GetWidget(b, 'type')
            if w:
                for item in self.BOXES.keys():
                    w.addItem(item)

    def setBase(self, parent):
        self.parent = parent

    def getExtension(self):
        return self.EXTENSION

    def getElementType(self, element={}):
        try:
            eltype = element.get('type') or \
                        self.getCurrentConfig()['type']
        except KeyError:
            eltype = self.BOXES.keys()[0]
        return eltype.lower()

    def getFieldElementType(self, element={}):
        eltype = self.getElementType(element)
        feltype = self.ui.Type.getValue().lower()
        if eltype != feltype:
            eltype = feltype
        return eltype

    def getBoxesByElement(self, eltype=None):
        if not eltype or eltype not in [l.lower() for l in self.BOXES.keys()]:
            eltype = self.getFieldElementType()
        try:
            boxes = self._BOXES[eltype.lower()]
        except KeyError:
            boxes = self._BOXES[self._BOXES.keys()[-1]]
        return map(lambda x: getattr(self.ui, x), boxes)

    def clearFields(self):
        for el in self._Forms:
            RefillFields(el, {})

    def blockFields(self):
        eltype = self.getFieldElementType()
        map(lambda el: el.setDisabled(True), self._Forms)
        map(lambda el: el.setDisabled(False), self.getBoxesByElement(eltype))

    def reloadFiles(self):
        files = fileManager.getFilesList(config.general.get('configs_path'), self.EXTENSION.lower())
        self.parent.ui.FilesList.clear()
        self.parent.ui.ItemsList.clear()
        self.clearFields()
        self._loadedConfig = []
        self._loadedFile = ''
        self._loadedElement = ''
        if not files or len(files) == 0:
            return
        for filename in files:
            QtGui.QListWidgetItem(filename, self.parent.ui.FilesList)


    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadElements(self, item):
        self._loadedConfig = []
        self._loadedElement = ''
        self.parent.ui.ItemsList.clear()
        if not item:
            return
        #if not luadata or type(luadata) == QtGui.QListWidgetItem:
        luadata = os.path.join(config.path, config.general.get('configs_path'), str(item.text()))
        self._loadedFile = str(item.text())
        data = lua.load(luadata)
        if not data:
            return
        for value in data:
            if type(value) is dict:
                try:
                    n = value['name']
                except KeyError:
                    try:
                        n = value['id']
                    except KeyError:
                        print "Bad record in file: %s" % value
                        continue
                QtGui.QListWidgetItem(str(n), self.parent.ui.ItemsList)
            else:
                print 'Not a dictionary: %s' % value
        self._loadedConfig = data


    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item):
        if not self._loadedConfig:
            return
        if not item:
            return
        self.clearFields()
        item = str(item.text()).lower()
        data = self._loadedConfig
        element = None
        for el in data:
            if type(el) is dict:
                if item in map(lambda x: str(x).lower(),
                                    (el.get('name'), el.get('id'))):
                    element = el
                    break
        if not element:
            return
        self._loadedElement = str(element.get('id') or element.get('name'))
        eltype = self.getElementType(element)
        for el in self.getBoxesByElement(eltype):
            RefillFields(el, element)

    def getCurrentElement(self):
        if not self._loadedElement:
            return None
        element = filter(lambda x: \
            str(x.get('id') or x.get('name')) == self._loadedElement,
            self._loadedConfig)
        return element[-1]

    def getCurrentConfig(self):
        try:
            return filter(lambda x: self._loadedElement == x.get('id') \
                        or x.get('name'), self._loadedConfig)[0]
        except IndexError:
            return {}

    def appendConfig(self, config):
        if type(config) is not dict or \
            config.get('name', None) is None:
            return
        if not self._loadedConfig:
            self._loadedConfig = [config,]
        else:
            self._loadedConfig.append(config)


    def saveFile(self):
        if not self._loadedFile:
            return
        data = self._loadedConfig
        if not data:
            return
        globalname = self._loadedElement
        eltype = self.getFieldElementType()
        saved = self.getSaved(eltype)
        if not saved:
            return
        elname = re.sub('\s+', '_', (saved.get('name') or ''
            ).strip()) or self._loadedElement
        if not 'id' in saved or not saved['id']:
            saved['id'] = '%s_%s_%s' % (eltype.lower(), elname, randint(0, 999999))
        if not globalname:
            globalname = str(saved.get('id'))
        for el in range(0, len(data)):
            if type(data[el]) is dict:
                if globalname.lower() in map(lambda x: str(x).lower(),
                            (data[el].get('name'), data[el].get('id'))):
                    data[el].update(saved) # Write to exists record
                    saved = {}
                    break
        if len(saved) > 0: # No record with such name. Add new
            data.append(saved)
        lua.dump(os.path.join(config.path,
            config.general.get('configs_path'), self._loadedFile), data)
        return elname

    def getSaved(self, eltype):
        boxes = self.getBoxesByElement(eltype)
        if not boxes:
            print "No boxes to save for %s" % eltype
            return
        saved = {}
        for box in boxes:
            for child in box.children()[1:]:
                try:
                    field = child.getValue()
                except Exception, e:
                    print str(e)
                    field = GetField(child)
                if field is not None:
                    name = re.sub('_\d+$', '', str(child.objectName()).lower())
                    saved[name] = field
        return saved
