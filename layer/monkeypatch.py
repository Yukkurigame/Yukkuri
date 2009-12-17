# $Id: monkeypatch.py 404 2009-04-23 07:32:59Z joe.wreschnig $

#!!! layer.monkeypatch cannot depend on any other layer modules. !!!#
# This shouldn't be a big deal because if we need to monkeypatch
# ourself, we're doing something horribly wrong.

import os
import sys
import re
import __builtin__

def makedirs(path, mode=511, makedirs=os.makedirs):
    """Create a leaf directory and all intermediate ones.

    This version does not fail if the leaf exists.
    """
    if not os.path.isdir(path):
        makedirs(path)
os.makedirs = makedirs

def re_escape(pattern, bad="/.^$*+?{,\\[]|()<>#=!:", escape=re.escape):
    """Escape characters with special meaning in the pattern.

    Python's default re.escape function escapes all non-ASCII which
    is not usually the desired behavior.
    """
    needs_escape = lambda c: (c in bad and "\\" + c) or c
    return "".join(map(needs_escape, pattern))
re.escape = re_escape

# Only present in 2.6+.
try: sys.getsizeof
except AttributeError:
    def getsizeof(obj, default=None):
        try: return len(obj.__dict__)
        except AttributeError:
            try: return len(obj.__slots__)
            except AttributeError: return len(dir(obj))
    sys.getsizeof = getsizeof

gettext = lambda value: value
ngettext = lambda v1, v2, count: (count == 1) and v1 or v2
__builtin__.__dict__.setdefault("_", gettext)
__builtin__.__dict__.setdefault("Q_", gettext)
__builtin__.__dict__.setdefault("N_", gettext)
__builtin__.__dict__.setdefault("ngettext", ngettext)

def clamp(v, l, h):
    if v < l: return l
    if v > h: return h
    return v
if clamp != __builtin__.__dict__.setdefault("clamp", clamp):
    raise TypeError("Unable to install clamp function.")


