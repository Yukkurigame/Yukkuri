# $Id: colors.py 392 2009-04-21 06:47:48Z joe.wreschnig $

import layer.load
import layer.command

__all__ = ["names", "add", "parse_color", "name", "value"]

__doc__ = """Color management.

This module contains routines for managing colors by name or other
string representation. To add color names during the Layer loading
process, place a .colors file in an indexed folder. This file
should list one color per line in any of the following formats:

#ff0000       Red
255 0 0       Red
#f00          Red
255, 0, 0     Red

Names are case-insensitive and unique, though the same color may have
many names.

"""

__color_to_name = {}
__name_to_color = {}

def iround(v):
    return int(round(v))

def names(only_canonical=False):
    """Return a list of available color names.

    If only canonical is set, then color names that map to the same
    value will only return one of the names.
    """
    if only_canonical:
        return __color_to_name.values()
    else:
        return __name_to_color.keys()

@layer.command.command(name="color_add", declspec=[str, int, int, int])
def add(name, r, g=None, b=None):
    """Add a name/color pair."""
    name = name.lower()
    if name in __name_to_color:
        raise KeyError("%r already exists as a color name" % name)
    if g is None or b is None:
        r, g, b = r
    (r, g, b) = (max(min(255, iround(r)), 0),
                 max(min(255, iround(g)), 0),
                 max(min(255, iround(b)), 0))
    __name_to_color[name] = (r, g, b)
    return __color_to_name.setdefault((r, g, b), name)

def parse_color(name):
    """Parse a color string.

    The string may be any of the formats allowed in a .colors file.
    """
    if name[0] == "#":
        value = int(name[1:], 16)
        if len(name) == 7:
            return (((value >> 16) & 0xFF),
                    ((value >> 8) & 0xFF),
                    ((value >> 0) & 0xFF))
        elif len(name) == 4:
            return (((value >> 8) & 0xF) * 0x11,
                    ((value >> 4) & 0xF) * 0x11,
                    ((value >> 0) & 0xF) * 0x11)
    else:
        try: r, g, b = name.replace(",", " ").split()
        except Exception: pass
        else:
            return (max(min(255, int(r)), 0),
                    max(min(255, int(g)), 0),
                    max(min(255, int(b)), 0))
    raise ValueError("%r is not a value color name" % name)

def load():
    """Load all .colors files."""
    print_d("Loading colors...")
    for filename in layer.load.glob("*.colors"):
        for line in layer.load.file(filename, "rU"):
            try:
                line = line.strip()
                if line.startswith("#"):
                    value, name = line.split(None, 1)
                    color = parse_color(value)
                else:
                    r, g, b, name = line.split(None, 3)
                    color = (max(min(255, int(r)), 0),
                             max(min(255, int(g)), 0),
                             max(min(255, int(b)), 0))
                add(name, color)
            except Exception, err:
                print_d("Unable to parse color  %r" % line)
    print_d("\tDone. (%d names, %d values)" % (
        len(__name_to_color), len(__color_to_name)))

def unload():
    """Clear all name/color pairs.

    This function is intended for testing and should not need to be
    called during normal operation.
    """
    __color_to_name.clear()
    __name_to_color.clear()

@layer.command.command(name="color_value")
def value(name, alpha=None):
    """Return the (r, g, b) color triple for a name."""
    if alpha is None:
        alpha = ()
    else:
        alpha = (alpha,)
    if not name:
        return (0, 0, 0) + alpha
    name = name.lower()
    try: return __name_to_color[name] + alpha
    except KeyError:
        return parse_color(name) + alpha

@layer.command.command(name="color_name", declspec=[int, int, int])
def name(r, g=None, b=None):
    """Return a name of an (r, g, b) triple.

    If a name is not found, a #rrggbb format string is returned.
    """
    if g is None or b is None:
        r, g, b = r
    try: return __color_to_name[(r, g, b)]
    except KeyError: return "#%02x%02x%02x" % (
        min(255, max(0, r)), min(255, max(0, g)), min(255, max(0, b)))

def rgb_to_xyz((r, g, b)):
    """Convert an RGB triple to an XYZ triple."""
    r /= 255.0
    g /= 255.0
    b /= 255.0
    if (r > 0.04045): r = ((r + 0.055) / 1.055) ** 2.4
    else: r /= 12.92
    if (g > 0.04045): g = ((g + 0.055) / 1.055) ** 2.4
    else: g /=  12.92
    if (b > 0.04045): b = ((b + 0.055) / 1.055) ** 2.4
    else: b /=  12.92
    return (r * 0.4124 + g * 0.3576 + b * 0.1805,
            r * 0.2126 + g * 0.7152 + b * 0.0722,
            r * 0.0193 + g * 0.1192 + b * 0.9505)

def xyz_to_rgb((x, y, z)):
    """Convert an XYZ triple to an RGB triple."""
    r = x *  3.2406 + y * -1.5372 + z * -0.4986
    g = x * -0.9689 + y *  1.8758 + z *  0.0415
    b = x *  0.0557 + y * -0.2040 + z *  1.0570
    if r > 0.0031308: r = 1.055 * (r ** (1/2.4)) - 0.055
    else: r *= 12.92
    if g > 0.0031308: g = 1.055 * (g ** (1/2.4)) - 0.055
    else: g *= 12.92
    if b > 0.0031308: b = 1.055 * (b ** (1/2.4)) - 0.055
    else: b *= 12.92

    return (iround(r * 255), iround(g * 255), iround(b * 255))

def _interp3((a1, b1, c1), (a2, b2, c2), p):
    q = 1 - p
    return (a1 * q + a2 * p,
            b1 * q + b2 * p,
            c1 * q + c2 * p)

def interpolate(c1, c2, p):
    """Interpolate two colors."""
    # XYZ interpolation gives dramatically better results when
    # tweening between dissimilar colors than RGB interpolation.
    return xyz_to_rgb(_interp3(rgb_to_xyz(c1), rgb_to_xyz(c2), p))
