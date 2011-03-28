from PyQt4 import QtGui, QtCore
from framework import *

class EntityTab:
    
    def __init__(self, main):
        self.main = main
        main.connect(GetWidget(self.main.ui.EntityMainBox, 'image').children()[-1].children()[-1],
            QtCore.SIGNAL('clicked()'), self.OpenEntityImage)
        main.connect(main.ui.Bloodcolor.children()[-1], QtCore.SIGNAL('clicked()'), self.ChangeEntityColor)
        main.connect(GetWidget(main.ui.EntityMainBox, 'height').children()[-1], QtCore.SIGNAL("valueChanged(int)"),
            self.SetPerviewHeight)
        main.connect(GetWidget(main.ui.EntityMainBox, 'width').children()[-1], QtCore.SIGNAL("valueChanged(int)"),
            self.SetPerviewWidth)
        main.connect(main.ui.EntityAnimationChooser, QtCore.SIGNAL("currentIndexChanged(int)"),
            self.ChangeAnimaptionType)        
        self.__AnimationPreview = []
    
    def OpenEntityImage(self):
        self.main.OpenImage(GetWidget(self.nain.ui.EntityMainBox, 'image').children()[-2])
    
    def ChangeEntityColor(self):
        self.main.ChangeColor(self.main.ui.Bloodcolor)
    
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
        if not abox.isEnabled():
            abox.setDisabled(True)
            return
        else:
            abox.setDisabled(False)        
        animations = self.main.ui.EntityAnimationBox.children()[1:]        
        labels = map(lambda a: str(a.children()[1].text()).replace(":", ""), animations)
        self.__AnimationPreview = [PDict({'label': labels[i], 'widget': animations[i]}) for i in range(0, len(animations))]
        chooser.addItems(labels)
        self.ChangeAnimaptionType()

    def ChangeAnimaptionType(self):         
        atype = self.main.ui.EntityAnimationChooser.currentIndex()
        print atype
        try:
            self.__AnimationPreview[atype]
        except Exception, e:
            print e
            return
        animation = GetField(self.__AnimationPreview[atype].widget)
        if len(animation) < 2:
            self.main.ui.EntityAnimationFrame.children()[1].setMaximum(1)
            return
        print atype
        self.main.ui.EntityAnimationFrame.children()[1].setMinimum(1)
        self.main.ui.EntityAnimationFrame.children()[1].setMaximum(animation[1])
        self.main.ui.EntityAnimationFrame.children()[1].setValue(1)
        self.main.ui.EntityAnimationFrame.children()[-1].setValue(1)
        print atype
                    
    def ChooseAnimationFrame(self):
        pass
        