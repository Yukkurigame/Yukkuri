# $Id$

"""Generic, computed-once constants.

This is not a public interface, and should only be used by functions
that need access to these variables during Layer initialization. After
that point, they are all available in the root 'layer' module.
"""

import os
import sys
import locale

VERSION = (0, 1)
VERSION_STRING = ".".join(map(str, VERSION))

try: ENCODING = locale.getpreferredencoding()
except locale.Error: ENCODING = "utf-8"

# http://developer.gnome.org/doc/API/2.0/glib/glib-running.html
if "G_FILENAME_ENCODING" in os.environ:
    FSCODING = os.environ["G_FILENAME_ENCODING"].split(",")[0]
    if FSCODING == "@locale":
        FSCODING = ENCODING
elif "G_BROKEN_FILENAMES" in os.environ:
    FSCODING = ENCODING
else:
    FSCODING = "utf-8"

DEBUG = "LAYER_DEBUG" in os.environ or os.path.isdir(
    os.path.join(os.path.dirname(__file__), ".svn"))

if sys.version_info < (2, 5):
    raise SystemExit("Layer requires Python 2.5 or a later 2.x-series.")

__all__ = ["ENCODING", "FSCODING", "VERSION", "VERSION_STRING", "DEBUG"]
