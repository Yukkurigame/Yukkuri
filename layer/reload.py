# $Id: reload.py 416 2009-04-26 05:19:57Z joe.wreschnig $

import layer.load
import layer.parse
import layer.const

from layer.task import Task

import fnmatch
import os
import threading
import atexit

_registered = []

__all__ = ["register", "init"]

def register(pattern, func):
    """Register a function to be called when a file changes."""
    _registered.append((pattern.lower(), func))

def init():
    """Initialize a thread to check for changed files and reload them."""
    to_reload = []

    # Always do the actual update in the main thread, so game logic
    # doesn't get half old and half new data during a function.
    def update(dt):
        Kinds = []
        while to_reload:
            filename = to_reload.pop()
            if (filename.startswith(".")
                or filename.endswith("~")
                or ".svn" in filename
                ):
                continue
            for pattern, func in _registered:
                fnlower = filename.lower()
                if fnmatch.fnmatch(fnlower, pattern):
                    func(filename)

    Task(update, lower_bound=0.25, scalable=False).enable()

    # Do the filesystem notifications in a thread, since Windows
    # requires it and it gives us better accuracy / less ambient CPU.
    path = os.path.join(os.path.dirname(layer.load.dirname), "data")
    thread = threading.Thread(target=_init, args=(to_reload, path))
    thread.setDaemon(True)
    thread.start()

def _init(to_check, path):
    print_d("Initialized no-op reloader.")

##########################################################################
# Per-platform implementation details follow.

## Linux implementation, based on inotify.
try:
    #import pyinotify
    #pyinotify.VERBOSE = True
    from pyinotify import (WatchManager, ThreadedNotifier, EventsCodes,
                           ProcessEvent)
except ImportError:
    pass
else:
    class Process(ProcessEvent):
        def process_IN_MODIFY(self, event):
            if event.name not in self.to_reload:
                self.to_reload.append(event.name)
        process_IN_CREATE = process_IN_MODIFY

    def quit(notifier):
        print_d("Shutting down reloader.")
        notifier.stop()

    def _init(to_reload, path):
        mask = EventsCodes.IN_MODIFY | EventsCodes.IN_CREATE
        wm = WatchManager()
        wm.add_watch(path, mask, rec=True)
        process = Process()
        process.to_reload = to_reload
        notifier = ThreadedNotifier(wm, process)
        notifier.start()
        atexit.register(quit, notifier)
        print_d("Initialized inotify reloader.")
        
## Win32 implementation, using Win32 Python.
try:
    import win32con
    import win32file
except ImportError: pass
else:
    def _init(to_reload, path):
        hDir = win32file.CreateFile(
            path, 0x1,
            win32con.FILE_SHARE_READ | win32con.FILE_SHARE_WRITE,
            None,
            win32con.OPEN_EXISTING,
            win32con.FILE_FLAG_BACKUP_SEMANTICS,
            None
            )
        while True:
            results = win32file.ReadDirectoryChangesW(
                hDir, 1024, True,
                win32con.FILE_NOTIFY_CHANGE_FILE_NAME |
                win32con.FILE_NOTIFY_CHANGE_DIR_NAME |
                win32con.FILE_NOTIFY_CHANGE_SIZE |
                win32con.FILE_NOTIFY_CHANGE_LAST_WRITE,
                None, None)
            for action, name in results:
                if name not in to_reload:
                    to_reload.append(name)

def reload_reindex(filename):
    layer.load.reindex()
register("*", reload_reindex)
                    
def reload_ParseObject(filename):
    filename = filename.lower()
    for Kind in layer.parse.ParseObject.LoadableKinds:
        if fnmatch.fnmatch(filename, Kind.load_spec):
            Kind.Load()
register("*", reload_ParseObject)

def reload_const(filename):
    layer.const.load(filename, allow_duplicates=True)
register("*.const", reload_const)

def reload_font(filename):
    layer.load.add_font(filename)
register("*.ttf", reload_font)
