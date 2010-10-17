import os
import json
from slpp import SLPP

class Files:

    def __init__(self):
        self.folder = ''
        self.last = ''
    
    #ororororororo
    def setFolder(self, folder):
        self.folder = folder
        self.last = folder
    
    def setLast(self, folder):
        self.last = folder
    
    def getLast(self):
        if not self.last:
            return ''
        return self.last

    def getFolder(self):
        if not self.folder:
            return ''
        return self.folder

    def getFilesList(self, extension=None):
        if not self.folder: return
        files = os.listdir(self.folder)
        f = []
        if extension:
            for name in files:
                if name.rfind('.'+extension) == len(name) - len(extension) - 1:
                    f.append(name)
        else:  f = files
        return f

class Config(dict):

    def __init__(self, indict=None):
        if indict is None:
            indict = {}
        dict.__init__(self, indict)
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
        stream = file('config', 'w')
        json.dump(dict(self), stream)

    def load(self):
        try: stream = file('config', 'rU')
        except: pass
        else:
            d = json.load(stream)
            for key in d.keys():
                setattr(self, key, d[key])

class Lua:

    def dump(self, filename, data):
        text = SLPP().encode(data)
        #print text
        #return
        if text:
            stream = file(filename, 'w')
            stream.write(text)
            stream.close()

    def load(self, filename):
        try: stream = file(filename, 'rU')
        except: return
        lf = stream.read()
        stream.close()
        data = SLPP().decode(lf)
        return data

filesManager = Files()
config = Config()
lua = Lua()