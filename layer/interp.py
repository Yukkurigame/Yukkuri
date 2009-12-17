# $Id: interp.py 391 2009-04-21 05:52:19Z joe.wreschnig $

from bisect import bisect_left

from layer.parse import ParseObject

def _interp(v1, v2, p, q):
    if v1 is not None and v2 is not None:
        try:
            interp = v1.interpolate
        except AttributeError:
            Kind = type(v1)
            if isinstance(v1, list) or isinstance(v1, tuple):
                v = Kind([_interp(a, b, p, q) for (a, b) in zip(v1, v2)])
            else:
                v = Kind(v1 * q + v2 * p)
        else:
            v = interp(v2, p)
        return v
    elif v1 is None:
        return v2
    else:
        return v1

class Interpolatable(object):
    _interpolate_ = []

    def interpolate(self, other, p, result=None):
        if result is None:
            result = type(self)()
        if p <= 0:
            result.__dict__.update(self.__dict__)
            return result
        elif p >= 1:
            result.__dict__.update(other.__dict__)
            return result
            
        attribs = self._interpolate_
        q = 1 - p
        # Fill in default values
        result.__dict__.update(self.__dict__)
        result.__dict__.update(other.__dict__)
        for attr in attribs:
            v1 = getattr(self, attr)
            v2 = getattr(other, attr)
            setattr(result, attr, _interp(v1, v2, p, q))
        return result

class InterpFunction(ParseObject):
    """Interpolate a discrete set of values over a continuous interval."""

    # TODO: (if necessary)
    # - More interpolation modes (quad, cubic)
    # - Smart interpolation (fit to data)
    # - Interpolate off the ends of the data set rather than flatline

    def __init__(self, name, values):
        self.name = name
        self.__values = list(values)
        self.__values.sort()

    def __call__(self, t):
        # FIXME: Some optimizations might be needed:
        # - memoize returns (esp. if t is int and not float)
        if t <= self.__values[0][0]:
            return self.__values[0][1]
        elif t >= self.__values[-1][0]:
            return self.__values[-1][1]

        i = bisect_left(self.__values, (t,))
        ax, av = self.__values[i - 1]
        bx, bv = self.__values[i]

        p = (t - ax) / (bx - ax)
        return av * (1 - p) + bv * p
