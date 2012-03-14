
from PyQt4 import QtCore
from sprites import manager as sprites

class YAnimationPreviewWidget(object):

    def __init__(self, parent):
        self.ui.AnimationViewer.setScrollPolicy(
            QtCore.Qt.ScrollBarAlwaysOff, QtCore.Qt.ScrollBarAlwaysOff)
        self.ui.comboBox.currentIndexChanged.connect(self.setFrames)
        self.connect(self.ui.Frame, QtCore.SIGNAL('valueChanged(int)'), self.setImage)
        self.connect(self.ui.Frame, QtCore.SIGNAL('valueChanged()'), self.setImage)
        self.data = None
        self._imageName = lambda: None
        self.__animationPreview = []

    @QtCore.pyqtSlot(list)
    def updateData(self, data):
        self.data = data
        self.updateTypes()

    #@QtCore.pyqtSlot(int)
    def setFrames(self, tp):
        if not self.data:
            return
        self.ui.Frame.setRange(*self.data[tp][1:])

    @QtCore.pyqtSlot()
    @QtCore.pyqtSlot(int)
    def setImage(self, index=None):
        typebox = self.ui.comboBox
        if not self.__animationPreview:
            self.__animationPreview = [[] for i in range(0, typebox.count())]
        atype = typebox.currentIndex()
        animpv = self.__animationPreview[atype]
        if index is None:
            index = self.ui.Frame.getValue()
        count = self.ui.Frame.diff()
        if len(animpv) != count:
            animpv.extend([None for i in range(0, count)])
        try:
            frame = animpv[index]
            if frame is None:
                raise IndexError
        except IndexError:
            frame = sprites.createPixmap(self._imageName(), index)
            animpv.insert(index, frame)
        if frame:
            self.ui.AnimationViewer.setPixmap(frame)

    def updateTypes(self):
        box = self.ui.comboBox
        box.clear()
        if not self.data:
            return
        for item in self.data:
            if item:
                box.addItem(item[0])
        box.setCurrentIndex(0)
        self.__animationPreview = []
