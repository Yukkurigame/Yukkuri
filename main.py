#!/usr/bin/env python
import os, sys
from PyQt4 import QtCore,QtGui
from Editor import Ui_Editor
from files import *
import re

tabsExtension = ['entity', 'widget', 'tiles', 'map']

config = Config()
lua = Lua()

class Main(QtGui.QMainWindow):
    
    def __init__(self):        
        QtGui.QMainWindow.__init__(self)        
        self.ui=Ui_Editor()
        self.ui.setupUi(self)        
        self.__files = Files()
        self.__loadedConfig = []
        self.__loadedFile = ''
        self.ReloadFolder(config.path)
        
        self.connect(self.ui.exitbox.children()[1], QtCore.SIGNAL('clicked()'), self.SaveFile )
        self.connect(self.ui.Image_3.children()[-1].children()[-1], QtCore.SIGNAL('clicked()'),
            self.OpenEntityImage)
        self.connect(self.ui.Bloodcolor.children()[-1], QtCore.SIGNAL('clicked()'),
            self.ChangeEntityColor)
        self.connect(self.ui.exitbox.children()[2], QtCore.SIGNAL('clicked()'),
            QtGui.qApp, QtCore.SLOT('quit()'))
        self.connect(self.ui.OpenFolderDBox.children()[1], QtCore.SIGNAL('clicked()'), self.OpenFolder)
        self.connect(self.ui.MainTabs, QtCore.SIGNAL('currentChanged(int)'), self.ReloadFiles)
        self.connect(self.ui.FilesList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), self.ReloadElements)
        self.connect(self.ui.ItemsList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), self.ReloadContent)
    
    def OpenFolder(self):
        folder = self.__files.getFolder()
        folder = QtGui.QFileDialog.getExistingDirectory(self, 'Open folder', folder)
        self.ReloadFolder(folder)
    
    def OpenEntityImage(self):
        name = os.path.basename(str(
                QtGui.QFileDialog.getOpenFileName(self, 'Choose file', self.__files.getFolder())))
        if not name: return
        self.ui.Image_3.children()[-2].setText(name)
    
    def ChangeEntityColor(self):
        color = self.GetField(self.ui.Bloodcolor)
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
        self.__files.setFolder(folder)
        config.path = str(folder)
        self.ReloadFiles()
    
    def ReloadFiles(self):
        tabindex = self.ui.MainTabs.currentIndex()
        files = self.__files.getFilesList(tabsExtension[tabindex])
        self.ui.FilesList.clear()
        self.__loadedConfig = []
        self.__loadedFile = ''
        if not files or len(files) == 0:
            return
        for filename in files:
            l = QtGui.QListWidgetItem(filename, self.ui.FilesList);
        self.ui.ItemsList.clear()
        self.ClearFields()
    
    def ReloadElements(self):
        self.__loadedConfig = []        
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
                if type(el['name']).__name__ == 'str' and el['name'].lower() == item:
                    element = el
                    break
        if not element: return
        if eltype == 'Entity' or eltype == 'Plant' or eltype == 'Corpse':
            for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityFeedBox,
                        self.ui.EntityMiscBox]:
                self.RefillFields(el, element)
            if element.has_key('animation'):
                self.RefillFields(self.ui.EntityAnimationBox, element['animation'])
            else: 
                self.RefillFields(self.ui.EntityAnimationBox, {})

    def RefillFields(self, element, data):
        for e in filter(lambda x: type(x).__name__ == 'QWidget', element.children()):
            name = str(e.objectName()).lower()
            name = re.sub('_\d+$', '', name)
            value = ''
            for key in data.keys():
                if key.lower() == name:
                    value = data[key]
            self.SetField(e, value)

    def SetField(self, widget, data):
        lines = filter(lambda x: type(x).__name__ == 'QLineEdit', widget.children())
        if type(data).__name__ != 'list' and type(data).__name__ != 'tuple': data = [data]
        for i in range(0, len(lines)):
            if i < len(data):
                lines[i].setText(str(data[i]))
            else:
                lines[i].setText('')
    
    def ClearFields(self):
        for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityFeedBox,
                        self.ui.EntityMiscBox, self.ui.EntityAnimationBox]:
            self.RefillFields(el, {})
    
    def SaveFile(self):
        if not self.__loadedFile: return
        data = self.__loadedConfig
        if not data:
            return
        eltype = self.__loadedConfig[0]
        boxes = self.GetBoxes(eltype)
        name = ''
        for box in boxes:
            for child in box.children():
                if re.match('name(_\d+)?', child.objectName(), re.I):
                    name = self.GetField(child)
                    break
        if not name: return
        for el in range(1, len(data)):
            if type(data[el]).__name__ == 'dict' and data[el].has_key('name'):
                if type(data[el]['name']).__name__ == 'str' and data[el]['name'].lower() == name.lower():
                    data[el] = {}
                    for box in boxes:
                        for child in box.children()[1:]:
                            field = self.GetField(child)                            
                            if field:
                                name = re.sub('_\d+$', '', str(child.objectName()).lower())
                                data[el][name] = field
                    if eltype == 'Entity':
                        data[el]['animation'] = {}
                        for child in self.ui.EntityAnimationBox.children()[1:]:
                            name = re.sub('_\d+$', '', str(child.objectName()).lower())
                            data[el]['animation'][name] = self.GetField(child)
                    break
        lua.dump(os.path.join(config.path, self.__loadedFile), data)
    
    def GetBoxes(self, eltype):
        fields = []
        if eltype == 'Entity':
            for el in [self.ui.EntityMainBox, self.ui.EntityEntityBox, self.ui.EntityMiscBox]:
                fields.append(el)
        elif eltype == 'Plant':
            for el in [self.ui.EntityMainBox, self.ui.EntityMiscBox, self.ui.EntityFeedBox]:
                fields.append(el)
        elif eltype == 'Corpse':
            for el in [self.ui.EntityMainBox, self.ui.EntityFeedBox]:
                fields.append(el)
        return fields

    
    def GetField(self, widget):
        lines = filter(lambda x: type(x).__name__ == 'QLineEdit', widget.children())
        ret = []
        for line in lines:
            ret.append(str(line.text()))
        if len(ret) < 1:
            return
        if len(ret) < 2:
            ret = ret[0]
        return ret
    
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
