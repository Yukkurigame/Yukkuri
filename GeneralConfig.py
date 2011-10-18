# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'GeneralConfig.ui'
#
# Created: Mon Oct 17 22:11:04 2011
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_GeneralConfig(object):
    def setupUi(self, GeneralConfig):
        GeneralConfig.setObjectName(_fromUtf8("GeneralConfig"))
        GeneralConfig.resize(370, 424)
        GeneralConfig.setWindowTitle(QtGui.QApplication.translate("GeneralConfig", "General config", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout_2 = QtGui.QVBoxLayout(GeneralConfig)
        self.verticalLayout_2.setContentsMargins(9, -1, -1, -1)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.VideoSettings = QtGui.QGroupBox(GeneralConfig)
        self.VideoSettings.setTitle(QtGui.QApplication.translate("GeneralConfig", "Video settings", None, QtGui.QApplication.UnicodeUTF8))
        self.VideoSettings.setObjectName(_fromUtf8("VideoSettings"))
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.VideoSettings)
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.windows_width = YSpinBoxInputWidget(self.VideoSettings)
        self.windows_width.setObjectName(_fromUtf8("windows_width"))
        self.verticalLayout_3.addWidget(self.windows_width)
        self.windows_height = YSpinBoxInputWidget(self.VideoSettings)
        self.windows_height.setObjectName(_fromUtf8("windows_height"))
        self.verticalLayout_3.addWidget(self.windows_height)
        self.maximum_frame_rate = YSpinBoxInputWidget(self.VideoSettings)
        self.maximum_frame_rate.setObjectName(_fromUtf8("maximum_frame_rate"))
        self.verticalLayout_3.addWidget(self.maximum_frame_rate)
        self.minimum_frame_rate = YSpinBoxInputWidget(self.VideoSettings)
        self.minimum_frame_rate.setObjectName(_fromUtf8("minimum_frame_rate"))
        self.verticalLayout_3.addWidget(self.minimum_frame_rate)
        self.verticalLayout_2.addWidget(self.VideoSettings)
        self.PathsSettings = QtGui.QGroupBox(GeneralConfig)
        self.PathsSettings.setTitle(QtGui.QApplication.translate("GeneralConfig", "Paths settings", None, QtGui.QApplication.UnicodeUTF8))
        self.PathsSettings.setObjectName(_fromUtf8("PathsSettings"))
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.PathsSettings)
        self.verticalLayout_4.setObjectName(_fromUtf8("verticalLayout_4"))
        self.configs_path = YSimpleInputWidget(self.PathsSettings)
        self.configs_path.setEnabled(True)
        self.configs_path.setObjectName(_fromUtf8("configs_path"))
        self.verticalLayout_4.addWidget(self.configs_path)
        self.images_path = YSimpleInputWidget(self.PathsSettings)
        self.images_path.setEnabled(True)
        self.images_path.setObjectName(_fromUtf8("images_path"))
        self.verticalLayout_4.addWidget(self.images_path)
        self.default_image_name = YSimpleInputWidget(self.PathsSettings)
        self.default_image_name.setEnabled(True)
        self.default_image_name.setObjectName(_fromUtf8("default_image_name"))
        self.verticalLayout_4.addWidget(self.default_image_name)
        self.fonts_path = YSimpleInputWidget(self.PathsSettings)
        self.fonts_path.setEnabled(True)
        self.fonts_path.setObjectName(_fromUtf8("fonts_path"))
        self.verticalLayout_4.addWidget(self.fonts_path)
        self.scripts_path = YSimpleInputWidget(self.PathsSettings)
        self.scripts_path.setEnabled(True)
        self.scripts_path.setObjectName(_fromUtf8("scripts_path"))
        self.verticalLayout_4.addWidget(self.scripts_path)
        self.verticalLayout_2.addWidget(self.PathsSettings)
        self.MiscSettings = QtGui.QGroupBox(GeneralConfig)
        self.MiscSettings.setTitle(QtGui.QApplication.translate("GeneralConfig", "Other", None, QtGui.QApplication.UnicodeUTF8))
        self.MiscSettings.setObjectName(_fromUtf8("MiscSettings"))
        self.verticalLayout_5 = QtGui.QVBoxLayout(self.MiscSettings)
        self.verticalLayout_5.setObjectName(_fromUtf8("verticalLayout_5"))
        self.widgets_z = YSpinBoxInputWidget(self.MiscSettings)
        self.widgets_z.setObjectName(_fromUtf8("widgets_z"))
        self.verticalLayout_5.addWidget(self.widgets_z)
        self.map_tile_size = YSpinBoxInputWidget(self.MiscSettings)
        self.map_tile_size.setObjectName(_fromUtf8("map_tile_size"))
        self.verticalLayout_5.addWidget(self.map_tile_size)
        self.map_default_tile = YSpinBoxInputWidget(self.MiscSettings)
        self.map_default_tile.setObjectName(_fromUtf8("map_default_tile"))
        self.verticalLayout_5.addWidget(self.map_default_tile)
        self.day_length = YSpinBoxInputWidget(self.MiscSettings)
        self.day_length.setObjectName(_fromUtf8("day_length"))
        self.verticalLayout_5.addWidget(self.day_length)
        self.action_interval = YSpinBoxInputWidget(self.MiscSettings)
        self.action_interval.setObjectName(_fromUtf8("action_interval"))
        self.verticalLayout_5.addWidget(self.action_interval)
        self.max_spawn = YSpinBoxInputWidget(self.MiscSettings)
        self.max_spawn.setObjectName(_fromUtf8("max_spawn"))
        self.verticalLayout_5.addWidget(self.max_spawn)
        self.max_edibles = YSpinBoxInputWidget(self.MiscSettings)
        self.max_edibles.setObjectName(_fromUtf8("max_edibles"))
        self.verticalLayout_5.addWidget(self.max_edibles)
        self.player_dies = YSpinBoxInputWidget(self.MiscSettings)
        self.player_dies.setObjectName(_fromUtf8("player_dies"))
        self.verticalLayout_5.addWidget(self.player_dies)
        self.verticalLayout_2.addWidget(self.MiscSettings)
        self.buttonBox = QtGui.QDialogButtonBox(GeneralConfig)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout_2.addWidget(self.buttonBox)

        self.retranslateUi(GeneralConfig)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), GeneralConfig.close)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), GeneralConfig.accept)
        QtCore.QMetaObject.connectSlotsByName(GeneralConfig)

    def retranslateUi(self, GeneralConfig):
        pass

from widgets import YSpinBoxInputWidget, YSimpleInputWidget
