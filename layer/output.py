# $Id: output.py 404 2009-04-23 07:32:59Z joe.wreschnig $

import sys
import __builtin__
import time
import traceback

from layer._c import DEBUG, ENCODING

__hooks = []

__all__ = ["hook", "unhook"]

def hook(callback):
    __hooks.append(callback)

def unhook(callback):
    try: __hooks.remove(callback)
    except ValueError: pass

def print_(string, frm="utf-8", prefix="", output=sys.stdout, log=None):
    if prefix:
        def decode_or_repr(s):
            try: return s.decode(frm)
            except UnicodeError: return repr(s)
        lines = map(decode_or_repr, string.splitlines())
        string = prefix + ("\n" + prefix).join(lines)

    if not isinstance(string, unicode):
        try: string = string.decode(frm)
        except UnicodeError:
            string = repr(string)

    for hook in __hooks:
        hook(string)

    if output:
        try: string = string.encode(ENCODING, "replace")
        except UnicodeError:
            string = repr(string)
        print >>output, string

    return string

def print_d(string, context=""):
    """Print debugging information."""
    if DEBUG:
        output = sys.stderr
    else:
        output = None

    if context and not isinstance(context, str):
        try:
            context = type(context).__name__
            context += "." + traceback.extract_stack()[-2][2] + ": "
        except AttributeError:
            context = "Unknown Context"

    timestr = "%0.2f" % time.time()
    string = "%s: %s%s" % (timestr[-6:], context, string)
    return print_(string, prefix="D: ", log="Debug", output=output)
    
def print_w(string):
    """Print warnings."""
    # Translators: "W" as in "Warning". It is prepended to
    # terminal output. APT uses a similar output format.
    return print_(string, prefix=_("W: "), log="Warnings", output=sys.stderr)

def print_e(string):
    """Print errors."""
    # Translators: "E" as in "Error". It is prepended to
    # terminal output. APT uses a similar output format.
    return print_(string, prefix=_("E: "), log="Errors", output=sys.stderr)

__builtin__.__dict__["print_"] = print_
__builtin__.__dict__["print_d"] = print_d
__builtin__.__dict__["print_e"] = print_e
__builtin__.__dict__["print_w"] = print_w
