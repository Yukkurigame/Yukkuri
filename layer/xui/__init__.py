# $Id: __init__.py 407 2009-04-25 10:36:35Z joe.wreschnig $

"""layer.xui -- External UI tools.

This module supports UI libraries external to the normal Layer
display. This is implemented in GTK+ and PyGTK.

"""

# GTK+ initialization

import gtk

import layer.task
import layer.xui.profiler
import layer.xui.editor

gtk.gdk.threads_init()

def _do_gtk(dt):
    while gtk.events_pending():
        gtk.main_iteration()

_task = layer.task.schedule(_do_gtk, pausable=False, stoppable=False)

layer.command.command(name="xui_stop", func=_task.disable,
                      doc="Disable XUI applications.")

layer.command.command(name="xui_start", func=_task.enable,
                      doc="Enable XUI applications.")

# Register here so GProfiler stays Layer-free.
layer.command.command(lambda: layer.xui.profiler.GProfiler().show(),
                      name="gprofiler",
                      doc="Start the interactive graphical profiler.")
