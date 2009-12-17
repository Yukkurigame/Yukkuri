# $Id: profiler.py 396 2009-04-22 04:58:43Z joe.wreschnig $

import time

from cProfile import Profile

try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

try: from pstats import Stats
except ImportError: Stats = None

from layer.command import command

__active = {}

DEFAULT="profile-%(start)s_%(end)s.txt"

@command
def profile_start(filename=DEFAULT):
    """Start a profiling session."""
    if filename in __active:
        return _("Already profiling %s.") % filename
    else:
        prof, start = __active[filename] = (Profile(), time.localtime())
        prof.enable()
        return _("Profiling session %s started.") % filename

@command
def profile_dump(filename=DEFAULT, sort='time'):
    """Dump profile statistics to a file."""
    try:
        prof, start = __active[filename]
    except KeyError:
        return _("Not profiling %s.") % filename
    else:
        if Stats is None:
            return _("pstats not found; try apt-get install python-profiler.")
        end = time.strftime("%F-%H.%M.%S", time.localtime())
        start = time.strftime("%F-%H.%M.%S", start)
        prof.disable()
        s = StringIO()
        stats = Stats(prof, stream=s)
        stats.sort_stats(sort).print_stats()
        if filename:
            filename = filename % dict(start=start, end=end)
            fileobj = open(filename, "w+")
        else:
            fileobj = None
        s.seek(0)
        if fileobj:
            print >>fileobj, s.getvalue()
            fileobj.close()
        else:
            print_(s.readlines())
        prof.enable()
        return _("Profile data written to %s.") % fileobj.name

@command
def profile_stop(filename=DEFAULT):
    """End a profiling session."""
    try:
        prof, start = __active[filename]
    except KeyError:
        return _("Not profiling %s.") % filename
    else:
        prof.disable()
        del(__active[filename])
        return _("Stopping profiling session %s.") % filename

@command
def profile(onoff=None, filename=DEFAULT):
    """Toggle a profiling session.

    Best used via a bind of '++profile'.
    """
    if onoff is None:
        return filename in __active
    elif onoff:
        return profile_start(filename)
    else:
        return "\n".join([profile_dump(filename), profile_stop(filename)])
