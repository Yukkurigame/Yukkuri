
import os
from PyQt4.QtGui import QPixmap
from config import config
from files import *

__all__ = ['manager', ]

class SpriteManager:

    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Files, cls).__new__(
                                    cls, *args, **kwargs)
        return cls._instance

    def __init__(self):
        self.images = None
        self.images_by_id = {}

    def getImages(self):
        if not self.images:
            self.loadImages()
        return self.images

    def getImagesById(self):
        if not self.images:
            self.loadImages()
        return self.images_by_id

    def getImageById(self, iid):
        iid = unicode(iid)
        if not iid:
            return
        if not self.images:
            self.loadImages()
        if iid in self.images_by_id:
            return self.images_by_id[iid]

    def loadImages(self):
        files = fileManager.getFilesList(config.general.get('configs_path'), 'sprites')
        path = os.path.join(config.path, config.general.get('configs_path'))
        data = []
        for filename in files:
            luadata = os.path.join(path, filename)
            data.extend(lua.load(luadata))
        self.images = data
        for image in data:
            if 'id' in image:
                self.images_by_id[image['id']] = image
            else:
                print "Image has no id: %s" % image

    def createPixmap(self, imgid, imgnum=0):
        img = self.getImageById(imgid)
        if not img:
            return
        imgnum = int(imgnum)
        image = QPixmap()
        name = img.get('image')
        result = image.load(
                os.path.join(config.path,
                             config.general.get('images_path'),
                             name))
        if not result:
            print "Cannot load image %s" % os.path.join(
                    config.path,
                    config.general.get('images_path'),
                    name)
            return
        width = img.get('width', 0)
        height = img.get('height', 0)
        columns = img.get('columns', 1)
        rows = img.get('rows', 1)
        colwidth = (width / columns)
        rowheight = (height / rows)
        row = imgnum / columns
        column = imgnum - row * columns
        x = img.get('offsetx', 0) + colwidth * column
        y = img.get('offsety', 0) + rowheight * row
        image = image.copy(x, y, colwidth, rowheight)
        return image

manager = SpriteManager()
