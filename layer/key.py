# $Id: key.py 262 2009-03-16 06:20:38Z piman $

import pyglet.window.key

g = globals()
for k in dir(pyglet.window.key):
    if k == k.upper():
        g[k] = getattr(pyglet.window.key, k)
del(g)
del(k)

def unparse(*keys):
    strparts = []
    for key in keys:
        if isinstance(key, basestring):
            strparts.append(key.lower().lstrip("_"))
        else:
            strparts.append(pyglet.window.key.symbol_string(key))
    return "+".join(strparts).lower()

def parse(keystr):
    """Parse a key string into its component parts."""
    keys = []
    for key in keystr.lower().split("+"):
        if key in "0123456789":
            key = "_" + key
        key = key.lower()
        keys.append(key)
    keys.sort()
    return keys
