from PyQt4 import QtGui, QtCore
from framework import *
from math import floor
from sprites import manager as sprites

class EntityTab:

    def __init__(self, main):
        self.main = main
        main.connect(main.ui.EntityAnimationChooser, QtCore.SIGNAL("currentIndexChanged(int)"),
            self.changeAnimaptionType)
        main.connect(main.ui.EntityAnimationFrame.children()[1], QtCore.SIGNAL("valueChanged(int)"),
            self.chooseAnimationFrame)

        self.main.ui.EntityAnimationViewer.setScrollPolicy(
            QtCore.Qt.ScrollBarAlwaysOff, QtCore.Qt.ScrollBarAlwaysOff)

        self.__AnimationPreview = []

    def setPerviewWidth(self):
        field = self.main.sender()
        width = field.value()
        widget = self.main.ui.EntityAnimationViewer
        widget.resize(width, widget.height())

    def setPerviewHeight(self):
        field = self.main.sender()
        height = field.value()
        self.main.ui.EntityAnimationViewer.resize(self.main.ui.EntityAnimationViewer.width(), height)

    def loadAnimationPreview(self):
        abox = self.main.ui.EntityAnimationBox
        chooser = self.main.ui.EntityAnimationChooser
        chooser.clear()
        self.__AnimationPreview = []
        animations = self.main.ui.EntityAnimationBox.children()[1:]
        labels = map(lambda a: str(a.children()[1].text()).replace(":", ""), animations)
        self.__AnimationPreview = [PDict({'label': labels[i], 'widget': animations[i], 'frames': []})
                                        for i in range(0, len(animations))]
        chooser.addItems(labels)

    def changeAnimaptionType(self):
        atype = self.main.ui.EntityAnimationChooser.currentIndex()
        try:
            self.__AnimationPreview[atype].frames = []
        except Exception, e:
            print e
            return
        animation = GetField(self.__AnimationPreview[atype].widget)
        if len(animation) < 2:
            self.main.ui.EntityAnimationFrame.children()[1].setMaximum(1)
            return
        self.main.ui.EntityAnimationFrame.children()[1].setMinimum(1)
        self.main.ui.EntityAnimationFrame.children()[1].setMaximum(animation[1])
        self.main.ui.EntityAnimationFrame.children()[1].setValue(1)
        self.chooseAnimationFrame()

    def chooseAnimationFrame(self):
        #FIXME: Rewrite to new sprite system
        atype = self.main.ui.EntityAnimationChooser.currentIndex()
        animpv = None
        index = None
        frame = None
        animation = None
        try:
            animpv = self.__AnimationPreview[atype]
            index = self.main.ui.EntityAnimationFrame.children()[1].value() - 1
            animation = GetField(self.__AnimationPreview[atype].widget)
            if index > animation[1] - 1:
                index = animation[1] - 1
            if index < 0:
                index = 0
        except Exception, e:
            print e
            return
        count = animation[1] - animation[0]
        if len(animpv.frames) != count:
            animpv.frames = [ None for i in range(0, count)]
        try:
            frame = animpv.frames[index]
            if frame is None:
                raise IndexError
        except IndexError:
            name = GetField(GetWidget(self.main.ui.EntityMainBox, 'image'))
            frame = sprites.createPixmap(name, animation[0] + index)
            animpv.frames.insert(index, frame)
        self.main.ui.EntityAnimationViewer.setPixmap(frame)
