# $Id: memory.py 364 2009-04-16 05:23:03Z joe.wreschnig $

from layer.command import command
from layer.parse import ParseObject

import gc
import sys

"""Track Python memory usage.

This module basic commands for memory management.
"""

@command(name="memory_dump")
def dump(filename):
    """Dump memory statistics to a file.

This prints a dump of object counts currently in memory, as well as
size.  The salient objects are those that have ever been instantiated,
and belong to any top-level module that has subclassed ParseObject,
which is the set of modules Layer is especially interested in. This
can take a long time and use a lot of memory.

The size is only valid in Python 2.6+; the count is always valid. A
poor guess is made for size in Python 2.5; a negative return value
indicates this code path.
"""
    fileobj = file(filename, "w")
    modules = dict(layer=None)
    counts = {}
    sizes = {}
    gc.disable()
    gc.collect()
    for obj in gc.get_objects():
        try: obj.__module__
        except AttributeError: pass
        else:
            if isinstance(obj, type):
                try: hash(obj)
                except TypeError: continue
                counts.setdefault(obj, 0)
                sizes.setdefault(obj, 0)
                if issubclass(obj, ParseObject):
                    try: module = obj.__module__.split(".")[0]
                    except AttributeError: pass
                    else: modules[module] = None
    for obj in gc.get_objects():
        t = type(obj)
        try: hash(t)
        except TypeError: continue
        if t in counts and t.__module__.split(".")[0] in modules:
            counts[t] += 1
            sizes[t] += sys.getsizeof(obj)

    stats = [(t.__module__ + "." + t.__name__, v, sizes[t])
             for t, v in counts.items()
             if t.__module__.split(".")[0] in modules and v]
    print >>fileobj, "%-60s %8s %9s" % ("Module / Class", "Count", "Size")
    print >>fileobj, "-" * 79
    for name, count, size in sorted(stats):
        print >>fileobj, "%-59s %8d %10d" % (name, count, size)
    gc.enable()
    fileobj.close()

command(gc.collect, name="memory_collect",
        doc="Run garbage collection immediately.")

@command(name="memory_garbage")
def garbage():
    return repr(gc.garbage)

@command(name="memory_instances")
def instances(match, filename=None):
    match = match.lower()
    found = []
    for obj in gc.get_objects():
        try: name = type(obj).__name__.lower()
        except AttributeError: continue
        if match in name.lower():
            found.append((obj, gc.get_referrers(obj)))
    if filename:
        fileobj = file(filename, "w")
        for obj, referrers in found:
            print >>fileobj, "(%x)%r: %r" % (id(obj), obj, map(object.__repr__, referrers))
        fileobj.close()
    else:
        for obj, referrers in found:
            print_d("(%x)%r: %r" % (id(obj), obj, map(object.__repr__, referrers)))
