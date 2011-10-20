
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

    def loadImages(self):
        files = fileManager.getFilesList(config.general.get('configs_path'), 'sprites')
        path = os.path.join(config.path, config.general.get('configs_path'))
        data = []
        for filename in files:
            luadata = os.path.join(path, filename)
            data.extend(lua.load(luadata))
        self.images = data
        for image in data:
            if image.has_key('id'):
                self.images_by_id[image['id']]
            else:
                print "Image has no id: %s" % image

manager = SpriteManager()
