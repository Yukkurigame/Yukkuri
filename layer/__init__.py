# $Id: __init__.py 455 2009-04-30 08:10:13Z joe.wreschnig $

import os
import sys
import traceback

__all__ = ["VERSION", "VERSION_STING", "NAME", "USERDIR", "FSCODING",
           "ENCODING", "init", "quit"]

NAME = "layer"
USERDIR = os.path.dirname(os.path.abspath(__file__))

from layer._c import *
import layer.output

print_d("Layer %s importing..." % VERSION_STRING)

# Use Psyco unless we're debugging, or on Windows -- it has known
# input issues with pyglet. However, if it's manually requested,
# force it on.
if (not (__debug__ or DEBUG or os.name == "nt")
    or "LAYER_PSYCO" in os.environ):
    try: import psyco
    except ImportError: pass
    else:
        print_d("Enabling Psyco.")
        psyco.full()

import layer.monkeypatch

import pyglet

try: sys.argv.remove("--debug-gl")
except ValueError: pass
else: pyglet.options["debug_gl"] = False

import layer.i18n
import layer.load
import layer.reload
import layer.parse

def init(name="layer", display_name=None, argv=[], size=(640, 480),
         multiwindow=False):
    """Initialize Layer and load all data.

    name - an internal name to use (e.g. for ~/. files)
    display_name - the name to use on the title bar
    argv - a list of unprocessed arguments, to interpret as commands
    size - initial display size

    Returns the display.
    """

    global NAME, USERDIR
    NAME = name.lower()
    USERDIR = pyglet.resource.get_settings_path(NAME)
    try: os.makedirs(USERDIR)
    except OSError: pass

    pyglet.options["shadow_window"] = multiwindow

    import layer.i18n

    layer.i18n.install(NAME)

    if display_name is None:
        display_name = name

    import layer.pygletpatch
    import layer.const
    import layer.event
    import layer.expr
    import layer.keybind
    import layer.splash
    import layer.tiles
    import layer.audio
    import layer.window
    import layer.colors
    import layer.prefs
    import layer.simpleanim

    if DEBUG:
        try: import layer.xui
        except ImportError: pass
        import layer.memory
        import layer.profiler

    for fn in layer.load.glob("*.const"):
        layer.const.load(fn)

    layer.colors.load()

    for font in layer.load.glob("*.ttf"):
        print_d("Adding font %r... " % font)
        layer.load.add_font(font)
        print_d("\tDone.")

    for Kind in layer.parse.ParseObject.LoadableKinds:        
        Kind.Load()

    if DEBUG:
        layer.reload.init()

    layer.event.listen("start", layer.prefs.load)
    layer.event.listen("quit", layer.prefs.save)
    layer.event.fire("init")
    pyglet.clock.schedule(layer.event.tick)
    window = layer.window.Window(
        width=size[0], height=size[1], caption=display_name)
    print_d(_("%s / Layer %s initialized.") % (display_name, VERSION_STRING))

    layer.command.command(window.screenshot)
    layer.command.bool_command(window, "fps", doc="Show FPS")
    layer.command.bool_command(window, "vsync", doc="Wait for vertical sync")
    layer.command.bool_command(window.input, "debug", name="input_debug",
                               doc="Echo key presses")

    layer.command.process_argv(argv)

    return window

def run(window, run=pyglet.app.run):
    layer.event.fire("start")
    run()
    layer.event.fire("quit")

@layer.command.command
def quit():
    """Exit the application."""
    pyglet.app.exit()

@layer.command.command(name="parse_reload")
def reload_manual(name):
    """Reload a set of resources."""
    for Kind in layer.parse.ParseObject.LoadableKinds:
        if Kind.__name__ == name:
            Kind.Load()

@layer.command.command(name="parse_dump")
def dump(name, filename=None):
    for Kind in layer.parse.ParseObject.LoadableKinds:
        if Kind.__name__ == name:
            f = file(filename or Kind.__name__ + ".yaml", "w")
            f.write(layer.parse.dump(Kind.Instances.values()))
            f.close()

@layer.command.command(declspec=[str])
def execfile(filename):
    """Execute each line of a file as a command."""
    if "." not in filename:
        filename = filename + ".cmdscript"
    fileobj = layer.load.file(filename)
    for line in fileobj:
        line = line.strip()
        if line and not line.startswith("#"):
            layer.command.run(line)

print_d(_("Layer %s imported.") % VERSION_STRING)
