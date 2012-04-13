
from YTreeDialog import YTreeDialog

from PyQt4 import QtCore, QtGui
from sprites import manager as sprites



class YImageChooser(object):

    def __init__(self, parent=None):
        self.cdialog = YTreeDialog(self)
        self.connect(self.ui.ImageChooseButton, QtCore.SIGNAL('clicked()'),
                    self.chooseImage)
        self.ui.ImageField.mousePressEvent = self.chooseImage

    def loadSprites(self):
        data = sprites.getImages()
        items = {}
        for item in data:
            t = item.get('image')
            if not t:
                continue
            if not items.has_key(t):
                items[t] = []
            items[t].append(item)
        self.cdialog.insertTree(items)

    def chooseImage(self, event=None):
        self.loadSprites()
        val = self.cdialog.getItem()
        if val:
            self.setValue(val)
