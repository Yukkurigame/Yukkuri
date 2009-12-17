# $Id: expr.py 375 2009-04-17 06:15:15Z joe.wreschnig $

from layer.command import command, run
import layer.const
import layer.parse

__all__ = ["EvalContext", "Expression", "set", "add", "delete", "get"]

class EvalContext(object):
    """Named contexts to evaluate Python expressions in.

    An EvalContext acts roughly like a dictionary, however it
    also supports attribute assignment to add things to the
    evaluated namespace.
    """

    def __init__(self, name, globals={}):
        self.name = name
        globals.setdefault("const", layer.const)
        globals.setdefault("run", run)
        self._globals = globals

    def __setitem__(self, attr, value):
        self._globals[attr] = value

    def __getitem__(self, attr):
        return self._globals[attr]

    def __delitem__(self, attr):
        del(self._globals[attr])

    def get(self, attr, default=None):
        return self._globals.get(attr, default)

    def update(self, d):
        self._globals.update(d)

    def compile(self, e, filename=__file__):
        """Compile the expression.

        This will return a compiled expression which can be passed to
        the evaluate method for faster execution than a raw string.

        It can also be used to check the syntax of an expression if it
        might be invalid, though it cannot check the vality of
        variable or function names.

        If a list or tuple is passed in, a list or tuple of compiled
        expressions is passed back. If a dictionary is passed in, a
        dictionary with the same keys normalized to lowercase and
        compiled values is passed back.
        """ 

        if isinstance(e, list) or isinstance(e, tuple):
            return type(e)(map(self.compile, e))
        elif isinstance(e, dict):
            return dict([(k.lower(), self.compile(v)) for (k, v) in e.items()])
        else:
            return Expression(e, context=self, filename=filename)

    def __repr__(self):
        return "%s(name=%r, %d globals)" %(
            type(self).__name__, self.name, len(self._globals))

    def as_dict(self):
        return self._globals

_default = EvalContext("Default Context")

class Expression(layer.parse.ParseScalar):
    """An expression and an evaluation context together.

    It is capable of optimizing expressions containing only bare
    Python constants (e.g. '5 * 12 + 9', 'max([1, 2, 3])'), but still
    preserving the original string for serialization.
    """

    parse_tag = "!py"

    __slots__ = ["context", "_result", "source", "code", "constant"]

    def __init__(self, source, context=_default, filename=__file__):
        # Support passing in an Expression.
        try: source = source.source
        except AttributeError: pass
        self.source = source

        if isinstance(self.source, basestring):
            # If we can compile this as an eval statement instead of
            # an exec statement, we should. Eval statements are
            # slightly faster, and give us a return value.
            try:
                self.code = compile(self.source, filename, "eval")
            except SyntaxError:
                self.code = compile(self.source, filename, "exec")
                self.constant = False
            else:
                # If it is an eval statement, see if it evaluates to a
                # raw constant; if so, store the constant.
                try:
                    self._result = eval(self.code, {}, {})
                except Exception:
                    self.constant = False
                else:
                    self.constant = True
        else:
            # If what's passed in is not a string, we should cache it
            # directly as the eval result.
            self._result = self.source
            self.code = None
            self.constant = True
        self.context = context

    def __getstate__(self):
        return str(self.source)

    def __setstate__(self, source):
        self.__init__(source)

    def __call__(__self, context=None, **kwargs):
        context = context or __self.context
        if __self.code is not None:
            return eval(__self.code, context._globals, kwargs)
        else:
            return __self._result

@command(declspec=[str, layer.parse.load])
def eval_set(key, value):
    """Add a value to the default context."""
    _default[key] = value
    return value

@command
def eval_del(key):
    """Remove a value from the default context."""
    del(_default[key])

set = _default.__setitem__
delete = _default.__setitem__
add = _default.__setitem__
get = _default.__getitem__

Compile = _default.compile

def Decompile(e):
    if isinstance(e, list) or isinstance(e, tuple):
        return map(Decompile, e)
    elif isinstance(e, dict):
        return dict([(k.lower(), Decompile(v)) for (k, v) in e.items()])
    else:
        try: return e.source
        except AttributeError: return e

@command(name=["eval", "py"], declspec=[Compile])
def py(expr):
    """Run a raw Python string."""
    return expr()
