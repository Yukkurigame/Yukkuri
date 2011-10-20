
from PyQt4 import QtCore, QtGui
from widgets.ui.Ui_YTreeDialog import Ui_YTreeDialog

class YTreeDialog(QtGui.QDialog):

    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.ui = Ui_YTreeDialog()
        self.ui.setupUi(self)

    def insertTree(self, tree):
        def addToTree(base, tree):
            if type(tree) in [list, tuple]:
                for subitem in value:
                    addToTree(base, subitem)
                    return
            else:
                item = QtGui.QTreeWidgetItem(base)
                if type(tree) is dict:
                    key = tree.get('name') or tree.get('id')
                    item._additionalId = tree.get('id')
                else:
                    key = tree
                item.setText(0, key)
                if not key:
                    print "Blank tree %s" % tree

        for key, value in tree.iteritems():
            item = QtGui.QTreeWidgetItem([key,])
            item.setExpanded(True)
            item.setFlags(item.flags() & ~QtCore.Qt.ItemIsSelectable)
            self.ui.treeWidget.addTopLevelItem(item)
            addToTree(item, value)

    def getItem(self):
        if self.exec_():
            item = self.ui.treeWidget.currentItem()
            try:
                return item._additionalId
            except AttributeError:
                return item.text(0)





