#!/usr/bin/env python
import os, sys
from PyQt4 import QtCore,QtGui
from map import *
from Editor import Ui_Editor
from framework import *
from files import *
import re

tabsExtension = ['entity', 'widget', 'tiles', 'map']

class Main(QtGui.QMainWindow):
    
    def __init__(self):        
        QtGui.QMainWindow.__init__(self)        
        self.ui=Ui_Editor()
        self.ui.setupUi(self)        
        self.__loadedConfig = []
        self.__loadedFile = ''
        self.__loadedElement = ''
        self.__Map = MapWindow(self)
        self.ReloadFolder(config.path)
        
        self.connect(self.ui.Image_3.children()[-1].children()[-1], QtCore.SIGNAL('clicked()'),
            self.OpenEntityImage)
        self.connect(self.ui.Image_4.children()[-1].children()[-1], QtCore.SIGNAL('clicked()'),
            self.OpenTilesImage)
        self.connect(self.ui.Bloodcolor.children()[-1], QtCore.SIGNAL('clicked()'), self.ChangeEntityColor)
        #ololoshenkilolo. FUFUFU is so FUFUFU. 
        self.connect(filter(lambda x: self.ui.exitbox.buttonRole(x) == 7, self.ui.exitbox.children()[1:])[0],
                    QtCore.SIGNAL('clicked()'), self.ReloadContent)
        self.connect(self.ui.exitbox, QtCore.SIGNAL('accepted()'), self.SaveFile )
        self.connect(self.ui.exitbox, QtCore.SIGNAL('rejected()'), QtGui.qApp, QtCore.SLOT('quit()'))
        self.connect(self.ui.OpenFolderDBox.children()[1], QtCore.SIGNAL('clicked()'), self.OpenFolder)
        self.connect(self.ui.MainTabs, QtCore.SIGNAL('currentChanged(int)'), self.ReloadFiles)
        self.connect(self.ui.FilesList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), self.ReloadElements)
        self.connect(self.ui.ItemsList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), self.ReloadContent)
        
        self.connect(self.ui.TilesOffestXSpin, QtCore.SIGNAL("valueChanged(double)"), self.ReloadTilesImage)
        self.connect(self.ui.TilesOffestYSpin, QtCore.SIGNAL("valueChanged(double)"), self.ReloadTilesImage)
    
    def OpenFolder(self):
        folder = filesManager.getFolder()
        folder = QtGui.QFileDialog.getExistingDirectory(self, 'Open folder', folder)
        self.ReloadFolder(folder)
    
    def OpenEntityImage(self):
        self.OpenImage(self.ui.Image_3.children()[-2])
    
    def OpenTilesImage(self):
        self.OpenImage(self.ui.Image_4.children()[-2])
        self.ReloadTilesImage()
    
    def ReloadTilesImage(self):
        image = CreatePixmap(GetField(GetWidget(self.ui.TilesMainBox, 'image')),
                            GetField(GetWidget(self.ui.TilesMainBox, 'offsetx')),
                            GetField(GetWidget(self.ui.TilesMainBox, 'offsety')))
        if not image: return
        ShowImage(image, self.ui.TileImageViewer)

    def OpenImage(self, element):
        name = os.path.basename(str(
            QtGui.QFileDialog.getOpenFileName(self, 'Choose file', filesManager.getFolder())))
        if not name: return
        element.setText(name)
    
    def ChangeEntityColor(self):
        color = GetField(self.ui.Bloodcolor)
        try:
            color = map(lambda x: int(x), color)
            color = QtGui.QColor(*color)
        except:
            color = QtGui.QColor("linen")
        color = QtGui.QColorDialog.getColor(color, self, "Choose color")
        if not color.isValid(): return
        color = color.getRgb()[:3]        
        colorle = filter(lambda x: type(x).__name__ == 'QLineEdit', self.ui.Bloodcolor.children())
        for i in range(0, len(colorle)):
            colorle[i].setText(str(color[i]))
    
    def ReloadFolder(self, folder):
        if folder == '':
            return
        self.ui.FolderPlace.setText(folder)
        filesManager.setFolder(folder)
        config.path = str(folder)
        self.ReloadFiles()
    
    def ReloadFiles(self):
        tabindex = self.ui.MainTabs.currentIndex()        
        files = filesManager.getFilesList(tabsExtension[tabindex])
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
    
    def ReloadElements(self):
        self.__loadedConfig = []
        self.__loadedElement = ''
        self.ui.ItemsList.clear()
        self.ClearFields()
        item = self.ui.FilesList.currentItem()
        if not item:
            return
        self.__loadedFile = str(item.text())
        data = lua.load(os.path.join(config.path, str(item.text())))
        if len(data) < 2:
            return
        for i in range(1, len(data)):
            if type(data[i]).__name__ == 'dict' and data[i].has_key('name'):
                QtGui.QListWidgetItem(str(data[i]['name']), self.ui.ItemsList);
        self.__loadedConfig = data

    def ReloadContent(self):
        if not self.__loadedConfig:
            return
        item = self.ui.ItemsList.currentItem()
        if not item: return
        item = str(item.text()).lower()
        eltype = self.__loadedConfig[0]
        data = self.__loadedConfig[1:]
        element = []
        for el in data:
            if type(el).__name__ == 'dict' and el.has_key('name'):
                if str(el['name']).lower() == item:
                    element = el
                    break
        if not element: return
        self.__loadedElement = item
        for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityFeedBox,
                   self.ui.EntityMiscBox, self.ui.TilesMainBox]:
            RefillFields(el, element)
        if element.has_key('animation'):
                RefillFields(self.ui.EntityAnimationBox, element['animation'])
        else: 
                RefillFields(self.ui.EntityAnimationBox, {})
        if eltype == "Tiles" and element.has_key('image'):
            self.ReloadTilesImage()
        elif eltype == "MapRegion":
            self.__Map.LoadRegion(element)
            

    def ClearFields(self):
        for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityFeedBox,
                   self.ui.EntityMiscBox, self.ui.EntityAnimationBox, self.ui.TilesMainBox]:
            RefillFields(el, {})
    
    def SaveFile(self):
        if not self.__loadedFile: return
        data = self.__loadedConfig
        if not data:
            return
        eltype = self.__loadedConfig[0]
        boxes = self.GetBoxes(eltype)
        name = self.__loadedElement
        if not name: return
        for el in range(1, len(data)):
            if type(data[el]).__name__ == 'dict' and data[el].has_key('name'):
                if str(data[el]['name']).lower() == name.lower():
                    data[el] = {}
                    if eltype == "MapRegion":
                        data[el] = self.__Map.dump()
                    else:
                        for box in boxes:
                            for child in box.children()[1:]:
                                field = GetField(child)
                                if field:
                                    name = re.sub('_\d+$', '', str(child.objectName()).lower())
                                    if box.objectName() == "EntityAnimationBox":
                                        if not data[el].has_key('animation'): data[el]['animation'] = {}
                                        data[el]['animation'][name] = field
                                    else:
                                        data[el][name] = field
                        break
        lua.dump(os.path.join(config.path, self.__loadedFile), data)
        self.ReloadElements()
    
    def GetBoxes(self, eltype):
        fields = []
        if eltype == 'Entity':
            for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityMiscBox,
                       self.ui.EntityAnimationBox]:
                fields.append(el)
        elif eltype == 'Plant':
            for el in [self.ui.EntityMainBox, self.ui.EntityMiscBox, self.ui.EntityFeedBox]:
                fields.append(el)
        elif eltype == 'Corpse':
            for el in [self.ui.EntityMainBox, self.ui.EntityFeedBox]:
                fields.append(el)
        elif eltype == 'Tiles':
            for el in [self.ui.TilesMainBox]:
                fields.append(el)
        return fields

    def test(self):
        print 'ololo'            

def main():
    app = QtGui.QApplication(sys.argv)
    window=Main()
    window.show()
    e = app.exec_()
    config.save()
    sys.exit(e)

if __name__ == "__main__":
    main()
