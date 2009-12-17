# $Id: const.py 332 2009-04-07 08:01:25Z joe.wreschnig $

import layer.load
import layer.parse

from layer.command import command

_disallowed = []

def _is_valid(name):
    return (name and name[0].isalpha() and name.replace("_", "").isalnum()
            and name not in _disallowed)

def _check_valid(name):
    if not _is_valid(name):
        raise KeyError("Constant: %r not a valid name." % name)

@command(declspec=[str, layer.parse.load], name="constant_add")
def add(name, value):
    """Add a named constant."""
    g = globals()
    _check_valid(name)
    if name.lower() not in g:
        g[name] = value
        g[name.lower()] = value
        g[name.upper()] = value
    else:
        raise KeyError("Constant: %r already exists." % name)

@command(declspec=[str, layer.parse.load], name="constant_set")
def set(name, value):
    """Set a named constant."""
    _check_valid(name)
    g = globals()
    if name.lower() in g:
        g[name] = value
        g[name.lower()] = value
        g[name.upper()] = value
    else:
        raise KeyError("Constant: %r not found." % name)

@command(name="constant_del")
def delete(name):
    """Delete a named constant."""
    _check_valid(name)
    g = globals()
    for k in [name, name.lower(), name.upper()]:
        try: del(g[k])
        except KeyError: pass

@command(name="constant_get")
def get(name):
    """Get the value of a named constant."""
    g = globals()
    try: return g[name.lower()]
    except KeyError: raise KeyError("Constant: %r not found." % name)

@command(name="constant_list")
def list(name=""):
    """List constants."""
    lines = []
    g = globals()
    for k in g:
        if k.lower() == k and k not in _disallowed:
            lines.append("%-20s %r" % (k, g[k]))
    return "\n".join(lines)

def load(filename, allow_duplicates=False):
    print_d("Loading constants: %r..." % filename)
    for line in layer.load.file(filename):
        line = line.strip()
        if line and line[0] != "#":
            key, value = line.split(None, 1)
            value = layer.parse.load(value)
            try: add(key, value)
            except KeyError, err:
                if allow_duplicates:
                    set(key, value)
                else:
                    print_e(str(err))
    print_d("\tDone.")

# Must be last.
_disallowed = dir() + ["_disallowed"]

