
import os
from config import config
from files import fileManager, lua

__all__ = ['manager', ]

class WidgetsManager:

    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Files, cls).__new__(
                                    cls, *args, **kwargs)
        return cls._instance

    def __init__(self):
        self.widgets = None
        self.widgets_by_id = {}

    def getWidgets(self):
        if not self.widgets:
            self.loadWidgets()
        return self.Widgets

    def getWidgetsById(self):
        if not self.widgets:
            self.loadWidgets()
        return self.widgets_by_id

    def getWidgetById(self, wid):
        if wid is not None:
            wid = unicode(wid)
        if not wid:
            return
        if not self.widgets:
            self.loadWidgets()
        if wid in self.widgets_by_id:
            return self.widgets_by_id[wid]

    def loadWidgets(self):
        files = fileManager.getFilesList(config.general.get('configs_path'), 'widget')
        path = os.path.join(config.path, config.general.get('configs_path'))
        data = []
        for filename in files:
            luadata = os.path.join(path, filename)
            data.extend(lua.load(luadata))
        self.widgets = data
        for widget in data:
            if 'id' in widget:
                self.widgets_by_id[widget['id']] = widget
            else:
                print "Widget has no id: %s" % widget

manager = WidgetsManager()
