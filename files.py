import os
import json
from slpp import slpp

class Files:

    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Files, cls).__new__(
                                    cls, *args, **kwargs)
        return cls._instance

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

    def getFilesList(self, path=None, extension=None):
        if not self.folder:
            return
        try:
            files = os.listdir(os.path.join(self.folder, path or 'data/defs'))
        except:
            pass
        else:
            f = []
            if extension:
                for name in files:
                    e = name.rfind('.'+extension)
                    if e > 0 and e == len(name) - len(extension) - 1:
                        f.append(name)
            else:
                f = files
            return f

class Lua:

    def dump(self, filename, data):
        text = slpp.encode(data)
        if text:
            stream = file(filename, 'w')
            stream.write(text)
            stream.close()

    def loadFile(self, fname):
        if not os.path.exists(fname):
            return
        return self.load(fname)

    def load(self, data):
        stream = None
        try:
            stream = file(data, 'rU')
        except IOError:
            lf = data
        except Exception, e:
            raise
        else:
            lf = stream.read()
        finally:
            if stream:
                stream.close()
        data = slpp.decode(lf)
        return data

fileManager = Files()
lua = Lua()
