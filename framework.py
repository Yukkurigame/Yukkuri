import re
from files import *
from PyQt4.QtGui import QPixmap, QLabel, QFrame
from PyQt4.QtCore import pyqtSignal, SIGNAL

def GetWidget(box, wname):
    for child in box.children()[1:]:
        name = re.sub('_\d+$', '', str(child.objectName()).lower())
        if name == wname.lower(): return child

def GetField(widget):
    ret = []
    lines = filter(lambda x: type(x).__name__ in ['QLineEdit', 'QSpinBox', 'QDoubleSpinBox', 'QCheckBox'],
                    widget.children())
    for line in lines:
        ltype = type(line).__name__
        t = ''
        if ltype == 'QLineEdit':
            t = str(line.text())
            if re.match("^\d+\.\d+$", t): t = float(t)
            elif re.match("^\d+$", t): t = int(t)
        elif ltype == 'QSpinBox':
            t = line.value()
        elif ltype == 'QDoubleSpinBox':
            t = line.value()
        elif ltype == 'QCheckBox':
            t = bool(line.isChecked())
        ret.append(t)
    if len(ret) < 1: return
    if len(ret) < 2: ret = ret[0]
    return ret

def SetField(widget, data):
    lines = filter(lambda x: type(x).__name__ in ['QLineEdit', 'QSpinBox', 'QDoubleSpinBox', 'QCheckBox'],
                    widget.children())
    if len(lines):
        if type(data).__name__ != 'list' and type(data).__name__ != 'tuple': data = [data]
        for i in range(0, len(lines)):
            ltype = type(lines[i]).__name__
            field = ''
            if i < len(data):
                field = data[i]
            if ltype == 'QLineEdit':
                lines[i].setText(str(field))
            elif ltype == 'QSpinBox':
                try: lines[i].setValue(int(field))
                except: pass
            elif ltype == 'QDoubleSpinBox':
                try: lines[i].setValue(float(field))
                except: pass
            elif ltype == 'QCheckBox':
                if not field: field = False
                lines[i].setChecked(int(field))


def RefillFields(element, data):
    for e in filter(lambda x: type(x).__name__ == 'QWidget', element.children()):
        name = str(e.objectName()).lower()
        name = re.sub('_\d+$', '', name)
        value = ''
        for key in data.keys():
            if key.lower() == name:
                value = data[key]
        SetField(e, value)



#################################
#           Images              #
#################################

def CreatePixmap(name, x = 0, y = 0, width = 64, height = 64):
    if not name: return
    image = QPixmap()
    result = image.load(os.path.join(config.path, '..', 'images', name))
    if not result: return            
    image = image.copy(x, y, width, height)
    return image

def ShowImage(image, target):
    target.resize(image.width(), image.height())
    target.setPixmap(image)
