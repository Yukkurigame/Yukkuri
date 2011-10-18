
from GeneralConfig import Ui_GeneralConfig
from PyQt4 import QtCore, QtGui
from files import *
from framework import RefillFields
import re

class GeneralConfigDialog(QtGui.QDialog):

    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.ui = Ui_GeneralConfig()
        self.ui.setupUi(self)
        self._configData = None
        self.loaded = False
        self.load()

    def load(self):
        if self.loaded:
            return True
        if not config.path:
            return
        try:
            self._configData = lua.load(os.path.join(config.path,
                config.general.get('configs_path') or 'data/defs',
                'general.config'))[0]
            config.general = self._configData
            self.loaded = True
            return True
        except IndexError:
            print "Bad main config"

    def show(self):
        QtGui.QDialog.show(self)
        if not self._configData or not self.load():
                return
        for el in filter(lambda x: type(x) == QtGui.QGroupBox, self.children()):
            RefillFields(el, self._configData)

    def accept(self):
        self.save()
        QtGui.QDialog.accept(self)

    def save(self):
        saved = {}
        for box in filter(lambda x: type(x) == QtGui.QGroupBox, self.children()):
            for child in box.children()[1:]:
                try:
                    field = child.getValue()
                except Exception, e:
                    print str(e)
                    field = GetField(child)
                if field:
                    name = re.sub('_\d+$', '', str(child.objectName()).lower())
                    saved[name] = field
        saved.update({"name": 'general', "type": 'config', "id": 'config_general'})
        self._configData = config.general = saved
        saved = [saved,]
        lua.dump(os.path.join(config.path,
            config.general.get('configs_path') or 'data/defs',
            'general.config'), saved)


class Config(dict):

    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Config, cls).__new__(
                                    cls, *args, **kwargs)
        return cls._instance

    def __init__(self, indict=None):
        if indict is None:
            indict = {}
        dict.__init__(self, indict)
        setattr(self, 'general', {})
        self.load()

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

    def save(self):
        if self.has_key('general'):
            del self['general']
        stream = file('config', 'w')
        json.dump(dict(self), stream)

    def load(self):
        try:
            stream = file('config', 'rU')
        except:
            pass
        else:
            d = json.load(stream)
            for key in d.keys():
                setattr(self, key, d[key])

config = Config()

__all__ = ['GeneralConfigDialog', 'config']
