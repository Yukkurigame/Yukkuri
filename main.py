#!/usr/bin/env python
import os
import re
import sys
import signal
from Editor import Ui_Editor
from entity import EntityTab
from framework import *
from files import *
from map import *
from random import randint
from PyQt4 import QtCore, QtGui
from config import config, GeneralConfigDialog

TABS_EXTENSION = ['sprites', 'entity', 'widget', 'tiles', 'map']

TYPES_BOXES_ORDER = ['SpritesMainBox', 'EntityMainBox',
               'WidgetMainBox', 'TilesMainBox', 'MapOptionsBox']

TYPES_BOXES = {'SpritesMainBox': ['sprite',],
               'EntityMainBox': ['entity', 'plant', 'corpse'],
               'WidgetMainBox': ['widget',],
               'TilesMainBox': ['tile',],
               'MapOptionsBox': ['region',]}

#Maybe it could be in Main.__init__?
ELEMENT_BOXES = {
    'sprite': ['SpritesMainBox',],
    'entity': ['EntityMainBox', 'EntityEntityBox', 'EntityMiscBox',
                'EntityAnimationBox'],
    'plant': ['EntityMainBox', 'EntityMiscBox', 'EntityFeedBox'],
    'corpse': ['EntityMainBox', 'EntityFeedBox'],
    'tiles': ['TilesMainBox',],
}

class Main(QtGui.QMainWindow):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.ui = Ui_Editor()
        self.ui.setupUi(self)
        self.generalConfig = GeneralConfigDialog(self)
        self.__loadedConfig = []
        self.__loadedFile = ''
        self.__loadedElement = ''
        self._Forms = [self.ui.SpritesMainBox, self.ui.EntityMainBox,
                    self.ui.EntityEntityBox, self.ui.EntityFeedBox,
                    self.ui.EntityMiscBox, self.ui.EntityAnimationBox,
                    self.ui.TilesMainBox]
        self.__Map = MapWindow(self)
        self.EntytyTab = EntityTab(self)
        self.ReloadFolder(config.path)

        #ololoshenkilolo. FUFUFU is so FUFUFU.
        self.connect(filter(lambda x: self.ui.exitbox.buttonRole(x) == 7, self.ui.exitbox.children()[1:])[0],
                    QtCore.SIGNAL('clicked()'), self.ReloadContent)
        self.connect(self.ui.exitbox, QtCore.SIGNAL('accepted()'), self.SaveFile )
        self.connect(self.ui.exitbox, QtCore.SIGNAL('rejected()'), QtGui.qApp, QtCore.SLOT('quit()'))
        self.connect(self.ui.OpenFolderDBox.children()[1], QtCore.SIGNAL('clicked()'), self.OpenFolder)
        self.connect(self.ui.NewConfigButton, QtCore.SIGNAL('clicked()'), self.CreateConfigFile)
        self.connect(self.ui.NewEntryButton, QtCore.SIGNAL('clicked()'), self.CreateConfigRecord)
        self.connect(self.ui.MainTabs, QtCore.SIGNAL('currentChanged(int)'), self.ReloadFiles)
        self.connect(self.ui.FilesList, QtCore.SIGNAL("itemActivated(QListWidgetItem*)"), self.ReloadElements)
        self.connect(self.ui.ItemsList, QtCore.SIGNAL("itemActivated(QListWidgetItem*)"), self.ReloadContent)

        self.ui.actionQuit.triggered.connect(QtGui.qApp.quit)
        self.ui.actionGeneralConfig.triggered.connect(self.generalConfig.show)

        for boxname in TYPES_BOXES.keys():
            try:
                box = getattr(self.ui, boxname)
            except AttributeError:
                continue
            if boxname == 'SpritesMainBox':
                GetWidget(box, 'image').setBasePath(os.path.join(
                    config.path, config.general.get('images_path')))
            w = GetWidget(box, 'type')
            if w:
                for item in TYPES_BOXES[boxname]:
                    w.addItem(item.capitalize())

    def OpenFolder(self):
        folder = QtGui.QFileDialog.getExistingDirectory(self, 'Open folder', fileManager.getLast())
        self.ReloadFolder(folder)

    def OpenTilesImage(self):
        self.ReloadTilesImage()

    def ReloadTilesImage(self):
        pass
        #image = CreatePixmap(GetField(GetWidget(self.ui.TilesMainBox, 'image')),
        #                    GetField(GetWidget(self.ui.TilesMainBox, 'offsetx')),
        #                    GetField(GetWidget(self.ui.TilesMainBox, 'offsety')))
        #if not image:
        #    self.ui.TileImageViewer.clear()
        #    return
        #ShowImage(image, self.ui.TileImageViewer)

    def CreateConfigFile(self):
        tabindex = self.ui.MainTabs.currentIndex()
        ext = str(TABS_EXTENSION[tabindex])
        fullpath =str(QtGui.QFileDialog.getSaveFileName(self,
                'File creation', fileManager.getLast(),
                '%s (%s)' % (ext.capitalize(), '*.%s' % ext)))
        if not fullpath:
            return
        if fullpath.rsplit('.', 1)[-1] != ext:
            fullpath += '.%s' % ext
        if not os.path.exists(fullpath):
            open(fullpath,'w').close()
        path, name = os.path.split(fullpath)
        self.ReloadFolder(config.path)
        item = self.ui.FilesList.findItems(name, QtCore.Qt.MatchExactly)[0]
        self.ui.FilesList.setCurrentItem(item)

    def CreateConfigRecord(self):
        if not self.ui.FilesList.currentItem():
            return
        name, ok = QtGui.QInputDialog.getText(self, 'New item', 'Name')
        if not ok:
            return
        conf = {'name': str(name)}
        if not self.__loadedConfig:
            self.__loadedConfig = [conf,]
        else:
            self.__loadedConfig.append(conf)
        #self.__loadedElement = str(name)
        item = QtGui.QListWidgetItem(str(name), self.ui.ItemsList)
        self.ui.ItemsList.setCurrentItem(item)
        self.ReloadContent()
        self.SaveFile()

    def ReloadFolder(self, folder):
        if folder == '':
            return
        self.ui.FolderPlace.setText(folder)
        fileManager.setFolder(folder)
        config.path = str(folder)
        self.ReloadFiles()

    def ReloadFiles(self, tabindex=None):
        if tabindex is None:
            tabindex = self.ui.MainTabs.currentIndex()
        files = fileManager.getFilesList(config.general.get('configs_path'), TABS_EXTENSION[tabindex])
        self.ui.FilesList.clear()
        self.__loadedConfig = []
        self.__loadedFile = ''
        self.__loadedElement = ''
        if not files or len(files) == 0:
            return
        for filename in files:
            QtGui.QListWidgetItem(filename, self.ui.FilesList)
        self.ui.ItemsList.clear()
        self.ClearFields()
        if tabindex != 3:
            self.__Map.hide()

    def ReloadElements(self, luadata=None):
        self.__loadedConfig = []
        self.__loadedElement = ''
        self.ui.ItemsList.clear()
        item = self.ui.FilesList.currentItem()
        if not item:
            return
        if not luadata or type(luadata) == QtGui.QListWidgetItem:
            luadata = os.path.join(config.path,
                config.general.get('configs_path'), str(item.text()))
        self.__loadedFile = str(item.text())
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
                QtGui.QListWidgetItem(str(n), self.ui.ItemsList)
            else:
                print 'Not a dictionary: %s' % value
        self.__loadedConfig = data

    def ReloadContent(self):
        if not self.__loadedConfig:
            return
        item = self.ui.ItemsList.currentItem()
        if not item:
            return
        self.BlockFields()
        self.ClearFields()
        item = str(item.text()).lower()
        data = self.__loadedConfig
        element = None
        for el in data:
            if type(el) is dict:
                if item in map(lambda x: str(x).lower(),
                                    (el.get('name'), el.get('id'))):
                    element = el
                    break
        if not element:
            return
        self.__loadedElement = element.get('id') or element.get('name')
        if element.has_key('animation'):
            RefillFields(self.ui.EntityAnimationBox, element['animation'])
            self.EntytyTab.LoadAnimationPreview()
            self.ui.EntityAnimaptionPreview.setDisabled(False)
        else:
            self.ui.EntityAnimaptionPreview.setDisabled(True)
            RefillFields(self.ui.EntityAnimationBox, {})
        eltype = self.GetElementType(element)
        for el in map(lambda x: getattr(self.ui, x),
                ELEMENT_BOXES[eltype.lower()]):
            RefillFields(el, element)
        if eltype == "tiles" and element.has_key('image'):
            self.ReloadTilesImage()
        elif eltype == "region":
            self.__Map.LoadRegion(element)

    def BlockFields(self):
        map(lambda el: el.setDisabled(True), self._Forms)
        map(lambda el: el.setDisabled(False), self.GetBoxes(
                self.GetElementType()))

    def ClearFields(self):
        self.ReloadTilesImage()
        for el in self._Forms:
            RefillFields(el, {})

    def SaveFile(self):
        if not self.__loadedFile:
            return
        data = self.__loadedConfig
        if not data:
            return
        globalname = self.__loadedElement
        eltype = self.GetElementType()
        boxes = self.GetBoxes(eltype)
        if not boxes:
            print "No boxes to save for %s" % eltype
            return
        saved = {}
        if eltype == "region":
            saved = self.__Map.dump()
        else:
            for box in boxes:
                for child in box.children()[1:]:
                    try:
                        field = child.getValue()
                    except Exception, e:
                        print str(e)
                        field = GetField(child)
                    if field:
                        name = re.sub('_\d+$', '', str(child.objectName()).lower())
                        if box.objectName() == "EntityAnimationBox":
                            if not saved.has_key('animation'): saved['animation'] = {}
                            saved['animation'][name] = field
                        else:
                            saved[name] = field
        elname = re.sub('\s+', '_', (saved.get('name') or ''
                ).strip()) or self.__loadedElement
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
            config.general.get('configs_path'), self.__loadedFile), data)
        self.ReloadElements()
        self.ui.ItemsList.setCurrentItem(self.ui.ItemsList.findItems(elname, QtCore.Qt.MatchExactly)[0])
        self.ReloadContent()

    def GetBoxes(self, eltype=None):
        if not eltype or eltype not in ELEMENT_BOXES.keys():
            eltype = self.GetElementType()
        boxes = ELEMENT_BOXES[eltype.lower()]
        return map(lambda x: getattr(self.ui, x), boxes)

    def GetCurrentConfig(self):
        try:
            return filter(lambda x: self.__loadedElement == x.get('id') \
                        or x.get('name'), self.__loadedConfig)[0]
        except IndexError:
            return {}

    def GetElementType(self, element={}):
        try:
            eltype = element.get('type') or \
                        self.GetCurrentConfig()['type']
        except KeyError:
            eltype = TYPES_BOXES[
                TYPES_BOXES_ORDER[self.ui.MainTabs.currentIndex()]][0]
        return eltype

    def test(self):
        print 'ololo'

def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QtGui.QApplication(sys.argv)
    window=Main()
    window.show()
    e = app.exec_()
    config.save()
    sys.exit(e)

if __name__ == "__main__":
    main()
