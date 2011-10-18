
import os
from files import fileManager
from PyQt4.QtCore import QObject, pyqtSignal, SIGNAL

from PyQt4 import QtCore, QtGui

class YFileInputWidget(QObject):

    def __init__(self, parent=None):
        self.connect(self.ui.dialogButtonBox, QtCore.SIGNAL('accepted()'),
                    self.openFile)
        self.basepath = None

    def setBasePath(self, path):
        self.basepath = path

    def openFile(self):
        fullpath = str(QtGui.QFileDialog.getOpenFileName(self,
                    'Choose file', fileManager.getLast()))
        path, name = os.path.split(fullpath)
        fileManager.setLast(path)
        if self.basepath:
            path = os.path.relpath(path, self.basepath)
        if not name:
            return
        self.setValue(os.path.join(path, name))
        self.emit(SIGNAL("valueChanged(QString)"), QtCore.QString(fullpath))
