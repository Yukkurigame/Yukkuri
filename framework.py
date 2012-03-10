import re
from files import *
from PyQt4 import QtGui
from PyQt4.QtCore import QRect, pyqtSignal, SIGNAL

class PDict(dict):
    def __init__(self, indict=None):
        if indict is None:
            indict = {}
        dict.__init__(self, indict)
    def __getattr__(self, item):
        try:
            return self.__getitem__(item)
        except KeyError:
            return ''
            #raise AttributeError(item)
    def __setattr__(self, item, value):
        if self.__dict__.has_key(item):
            dict.__setattr__(self, item, value)
        else:
            self.__setitem__(item, value)

def GetWidget(box, wname):
    for child in box.children()[1:]:
        name = re.sub('_\d+$', '', str(child.objectName()).lower())
        if name == wname.lower():
            return child

def GetField(widget):
    ret = []
    lines = filter(lambda x: type(x) in [QtGui.QLineEdit, QtGui.QSpinBox,
                        QtGui.QDoubleSpinBox, QtGui.QCheckBox],
                    widget.children())
    for line in lines:
        ltype = type(line)
        t = ''
        if ltype == QtGui.QLineEdit:
            t = str(line.text())
            if re.match("^\d+\.\d+$", t): t = float(t)
            elif re.match("^\d+$", t): t = int(t)
        elif ltype == QtGui.QSpinBox:
            t = line.value()
        elif ltype == QtGui.QDoubleSpinBox:
            t = line.value()
        elif ltype == QtGui.QCheckBox:
            t = bool(line.isChecked())
        ret.append(t)
    if len(ret) < 1: return
    if len(ret) < 2: ret = ret[0]
    return ret

def SetField(widget, data):
    lines = filter(lambda x: type(x) in [QtGui.QLineEdit, QtGui.QSpinBox,
                    QtGui.QDoubleSpinBox, QtGui.QCheckBox],
                    widget.children())
    if len(lines):
        if type(data) != list and type(data) != tuple: data = [data]
        for i in range(0, len(lines)):
            ltype = type(lines[i])
            field = ''
            if i < len(data):
                field = data[i]
            if ltype == QtGui.QLineEdit:
                lines[i].setText(str(field))
            elif ltype == QtGui.QSpinBox:
                if not field: field = 0
                try: lines[i].setValue(int(field))
                except: pass
            elif ltype == QtGui.QDoubleSpinBox:
                if not field: field = 0.0
                lines[i].setValue(float(field))
            elif ltype == QtGui.QCheckBox:
                if not field: field = False
                lines[i].setChecked(int(field))


def RefillFields(element, data):
    for el in filter(lambda x: isinstance(x, QtGui.QWidget), element.children()):
        name = str(el.objectName()).lower()
        name = re.sub('_\d+$', '', name)
        value = ''
        for key in data.keys():
            if key.lower() == name:
                value = data[key]
        try:
            el.setValue(value)
        except Exception, e:
            SetField(el, value)

#################################
#           Images              #
#################################

def CreatePixmap(name, x=0, y=0, width=64, height=64):
    if not name: return
    image = QtGui.QPixmap()
    result = image.load(os.path.join(config.path, '..', 'images', name))
    if not result: return
    image = image.copy(x, y, width, height)
    return image

def ShowImage(image, target):
    target.resize(image.width(), image.height())
    target.setPixmap(image)

#################################
#           Classes             #
#################################

class Sprite(dict):

    def __init__(self, position=None, image=None, depth=0):
        self.rect = None
        self.image = None
        self.z = depth
        self.setRect(position)
        self.setImage(image)

    def setRect(self, rect):
        if type(rect) is QRect:
            self.rect = rect

    def setImage(self, image):
        if type(image) == QtGui.QPixmap:
            self.image = image

    def __getattr__(self, item):
        result = None
        try:
            if item in ('x', 'y', 'width', 'height'):
                if self.rect != None:
                    result = getattr(self.rect, item)()
            else:
                result = self.__getitem__(item)
        except Exception, e:
            print e
        return result

    def __setattr__(self, item, value):
        try:
            if item in ('x', 'y', 'width', 'height'):
                if self.rect != None:
                    getattr(self.rect, 'set' + item.capitalize())(value)
            else:
                self.__setitem__(item, value)
        except Exception, e:
            print e
