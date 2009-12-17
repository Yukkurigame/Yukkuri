# $Id: util.py 406 2009-04-24 10:27:38Z joe.wreschnig $

def meta_repr_from_list(*args):
    def repr_from_list(obj):
        slots = []
        for arg in args:
            arg = arg.lstrip("_")
            try: value = getattr(obj, arg)
            except AttributeError:
                value = getattr(obj, "_" + arg, "<Unknown>")
            slots.append("%s=%r" % (arg, value))
        return "%s(%s)" % (type(obj).__name__, ", ".join(slots))
    return repr_from_list

def repr_from_init(obj):
    args = obj.__init__.im_func.func_code.co_names[
        :obj.__init__.im_func.func_code.co_argcount]
    return meta_repr_from_list(*args)(obj)

def repr_from_slots(obj):
    return meta_repr_from_list(*type(obj).__slots__)(obj)

class attrdict(dict):
    """A dictionary that allows sub-attribute addressing.

    That is, if foo.bar is 3, attrdict(dict(foo=foo))['foo.bar'] is 3.

    This is useful to get Python 3-style substring formatting, without
    needing to use the horrible {} syntax.

    However, it's slow.
    """
    __slots__ = ()
    def __getitem__(self, key):
        keys = key.split(".", 1)
        value = dict.__getitem__(self, keys[0])
        if len(keys) == 1:
            return value
        else:
            try: return eval(key, {keys[0]: value, '__builtins__':[]}, {})
            except (AttributeError, IndexError, KeyError):
                raise KeyError(key)
