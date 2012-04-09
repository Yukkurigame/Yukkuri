
from framework import RefillFields
import json
import re

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

__all__ = ['config']
