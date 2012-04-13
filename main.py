#!/usr/bin/env python
import os
import re
import sys
import signal
from Editor import Ui_Editor
from framework import *
from files import *
from PyQt4 import QtCore, QtGui
from config import config
from widgets import YGeneralConfigDialog


class Main(QtGui.QMainWindow):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.generalConfig = YGeneralConfigDialog(self)
        self.ui = Ui_Editor()
        self.ui.setupUi(self)

        for tab in self.ui.MainTabs.children()[0].children():
            try:
                tab.setBase(self)
            except AttributeError, e:
                pass #print e

        if type(config.tab) is int and \
                config.tab != self.ui.MainTabs.currentIndex():
            self.ui.MainTabs.setCurrentIndex(config.tab)
        self.reloadFolder(config.path)

        #ololoshenkilolo. FUFUFU is so FUFUFU.
        self.connect(filter(lambda x: self.ui.exitbox.buttonRole(x) == 7, self.ui.exitbox.children()[1:])[0],
                    QtCore.SIGNAL('clicked()'), self.reloadContent)
        self.connect(self.ui.exitbox, QtCore.SIGNAL('rejected()'), QtGui.qApp, QtCore.SLOT('quit()'))
        #self.connect(self, QtCore.SIGNAL('destroyed()'), QtGui.qApp, QtCore.SLOT('quit()'))

        self.ui.actionGeneralConfig.triggered.connect(self.generalConfig.show)

    @QtCore.pyqtSlot()
    def openFolder(self):
        folder = QtGui.QFileDialog.getExistingDirectory(self, 'Open folder', fileManager.getLast())
        self.reloadFolder(folder)

    @QtCore.pyqtSlot()
    def createConfigFile(self):
        ext = self.getCurrentTab().getExtension()
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
        self.reloadFolder(config.path)
        item = self.ui.FilesList.findItems(name, QtCore.Qt.MatchExactly)[0]
        self.ui.FilesList.setCurrentItem(item)

    @QtCore.pyqtSlot()
    def createConfigRecord(self):
        if not self.ui.FilesList.currentItem():
            return
        name, ok = QtGui.QInputDialog.getText(self, 'New item', 'Name')
        if not ok:
            return
        self.getCurrentTab().appendConfig({'name': str(name)})
        item = QtGui.QListWidgetItem(str(name), self.ui.ItemsList)
        self.ui.ItemsList.setCurrentItem(item)
        self.reloadContent(item)
        self.saveFile()

    def reloadFolder(self, folder):
        if folder == '':
            return
        self.ui.FolderPlace.setText(folder)
        fileManager.setFolder(folder)
        config.path = str(folder)
        self.reloadFiles()

    @QtCore.pyqtSlot(int)
    def reloadFiles(self, tabindex=None):
        self.getCurrentTab().reloadFiles()
        config.tab = self.ui.MainTabs.currentIndex()

    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadElements(self, item=None, luadata=None):
        self.getCurrentTab().reloadElements(item)

    @QtCore.pyqtSlot('QListWidgetItem*')
    def reloadContent(self, item=None):
        self.getCurrentTab().reloadContent(item)

    def saveFile(self):
        elname = self.getCurrentTab().saveFile()
        self.reloadElements(self.ui.FilesList.currentItem())
        try:
            item = self.ui.ItemsList.findItems(elname, QtCore.Qt.MatchExactly)[0]
            self.ui.ItemsList.setCurrentItem(item)
        except TypeError, e:
            print elname
            raise e
        else:
            self.reloadContent(item)

    def getCurrentTab(self):
        return self.ui.MainTabs.currentWidget()

    def test(self):
        print 'ololo'


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QtGui.QApplication(sys.argv)
    window = Main()
    window.show()
    e = app.exec_()
    config.save()
    #sys.exit(e)

if __name__ == "__main__":
    main()

