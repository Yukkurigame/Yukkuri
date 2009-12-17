# $Id: load.py 447 2009-04-30 05:30:12Z joe.wreschnig $

"""layer.load -- Simple resource loading support.

layer.load provides a thin wrapper around pyglet's resources
loaders.

Unlike pyglet's resource system, Layer's is case-insensitive to avoid
porting issues; case conflicts are reported as errors on systems with
case-sensitive filenames.

It also provides separate sound and music resource loaders, and has a
globbing interface for scanning all files in the index matching a
pattern.

The default folders indexed are data/defs, data/images, data/audio,
data/shared, and data/fonts.
'
"""

#!!! layer.load cannot depend on any other layer modules. !!!#

import os
import sys
import fnmatch

import pyglet.resource

ResourceNotFoundException = pyglet.resource.ResourceNotFoundException

__all__ = ["file", "image", "glob", "add_font", "sound", "music",
           "ResourceNotFoundException"]

def module_path( ):
    path = os.path.dirname(unicode(__file__, sys.getfilesystemencoding( )))
    if not path:
        path = os.path.dirname(unicode(sys.executable, sys.getfilesystemencoding( )))
        if path.find("layer") < 0:
            path = os.path.join(path, "layer")
    if path.find("library.zip") > 0:
        path = path.replace("library.zip\\", "")
    return path

dirname = module_path()
print dirname

# Pyglet apparently hates \s in paths, but /s are okay?
DIRS = ["/".join(["data", path]) for path in
        ["defs", "images", "audio", "shared", "fonts"]]
DIRS = [d for d in DIRS if os.path.isdir(os.path.join(dirname, "..", d))]

def caseinsensitive(func):
    def wrap(self, name, *args, **kwargs):
        return func(
            self, self._casemap.get(name.lower(), name), *args, **kwargs)
    return wrap

class LayerLoader(pyglet.resource.Loader):
    def reindex(self, *args, **kwargs):
        super(LayerLoader, self).reindex(*args, **kwargs)
        self._casemap = {}
        for name in self._index.keys():
            if "#" in name:
                del(self._index[name])
                continue
            if self._casemap.setdefault(name.lower(), name) != name:
                raise IOError(
                    "%r and %r differ only by case" %(
                    name, self._casemap[name.lower()]))

    def glob(self, spec):
        """Return all files in the index matching the glob pattern."""
        spec = spec.lower()
        #print dirname
        res = [resource for name, resource in self._casemap.iteritems()
                if fnmatch.fnmatch(name, spec)]
        #print res
        return res 

    @caseinsensitive
    def image(self, filename, *args):
        if "." not in filename:
            filename = filename + ".png"
        image = super(LayerLoader, self).image(filename, *args)
        image.filename = filename
        return image

    file = caseinsensitive(pyglet.resource.Loader.file)
    animation = caseinsensitive(pyglet.resource.Loader.animation)
    attributed = caseinsensitive(pyglet.resource.Loader.attributed)
    html = caseinsensitive(pyglet.resource.Loader.html)
    location = caseinsensitive(pyglet.resource.Loader.location)
    media = caseinsensitive(pyglet.resource.Loader.media)
    text = caseinsensitive(pyglet.resource.Loader.text)
    texture = caseinsensitive(pyglet.resource.Loader.texture)
    add_font = caseinsensitive(pyglet.resource.Loader.add_font)

    def sound(self, filename, cache={}):
        """Load a static sound.

        Normally this is cached and repeated requests for the same
        filename will result in the same static data. Pass cache=False
        to disable this behasvior.
        """
        try: return cache[filename.lower()]
        except TypeError:
            source = self.media(filename, streaming=False)
        except KeyError:
            source = self.media(filename, streaming=False)
            cache[filename.lower()] = source
            source.filename = filename
        return source

    def music(self, filename):
        """Load a streaming sound."""
        source = self.media(filename, streaming=True)
        source.filename = filename
        return source

_loader = LayerLoader(DIRS)

file = _loader.file
image = _loader.image
glob = _loader.glob
add_font = _loader.add_font
sound = _loader.sound
music = _loader.music
reindex = _loader.reindex
