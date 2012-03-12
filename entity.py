from PyQt4 import QtGui, QtCore
from framework import *
from math import floor

class EntityTab:

    def __init__(self, main):
        self.main = main
        #main.connect(GetWidget(main.ui.EntityMainBox, 'height').children()[-1], QtCore.SIGNAL("valueChanged(int)"),
        #    self.SetPerviewHeight)
        #main.connect(GetWidget(main.ui.EntityMainBox, 'width').children()[-1], QtCore.SIGNAL("valueChanged(int)"),
        #    self.SetPerviewWidth)
        main.connect(main.ui.EntityAnimationChooser, QtCore.SIGNAL("currentIndexChanged(int)"),
            self.ChangeAnimaptionType)
        main.connect(main.ui.EntityAnimationFrame.children()[1], QtCore.SIGNAL("valueChanged(int)"),
            self.ChooseAnimationFrame)

        self.__AnimationPreview = []

    def SetPerviewWidth(self):
        field = self.main.sender()
        width = field.value()
        widget = self.main.ui.EntityAnimationViewer
        widget.resize(width, widget.height())

    def SetPerviewHeight(self):
        field = self.main.sender()
        height = field.value()
        self.main.ui.EntityAnimationViewer.resize(self.main.ui.EntityAnimationViewer.width(), height)

    def LoadAnimationPreview(self):
        abox = self.main.ui.EntityAnimationBox
        chooser = self.main.ui.EntityAnimationChooser
        chooser.clear()
        self.__AnimationPreview = []
        animations = self.main.ui.EntityAnimationBox.children()[1:]
        labels = map(lambda a: str(a.children()[1].text()).replace(":", ""), animations)
        self.__AnimationPreview = [PDict({'label': labels[i], 'widget': animations[i], 'frames': []})
                                        for i in range(0, len(animations))]
        chooser.addItems(labels)

    def ChangeAnimaptionType(self):
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
        self.ChooseAnimationFrame()

    def ChooseAnimationFrame(self):
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
        if len(animpv.frames) < animation[1] - 1:
            animpv.frames = [ [] for i in range(0, animation[1])]
        try:
            frame = animpv.frames[index]
            frame.width()
        except Exception:
            width = GetField(GetWidget(self.main.ui.EntityMainBox, 'width'))
            height = GetField(GetWidget(self.main.ui.EntityMainBox, 'height'))
            cols = GetField(GetWidget(self.main.ui.EntityMainBox, 'imagecols'))
            rows = GetField(GetWidget(self.main.ui.EntityMainBox, 'imagerows'))
            col = (animation[0] + index) % cols
            row = floor(float(animation[0] + index)/float(cols))
            startx = col * width
            starty = row * height
            name = GetField(GetWidget(self.main.ui.EntityMainBox, 'image'))
            print name, startx, starty, width, height
            #Broken
            frame = CreatePixmap(name, startx, starty, width, height)
            animpv.frames.insert(index, frame)
        self.ShowAnimationPerviewImage(frame)

    def ShowAnimationPerviewImage(self, image):
        ShowImage(image, self.main.ui.EntityAnimationViewer)
