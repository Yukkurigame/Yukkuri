# $Id: command.py 427 2009-04-27 06:10:22Z joe.wreschnig $

#!!! layer.command should only depend on layer.lexer. !!!#

import traceback
import os.path

import layer.lexer

__all__ = [ "CommandContext", "command", "run",
            "Error", "CommandNotFoundError", "CommandArityError",
            "CommandTypeError"]

class Error(Exception): pass
class CommandNotFoundError(Error): pass
class CommandArityError(Error): pass
class CommandTypeError(Error): pass


AND, OR, DELIM = range(3)
CommandLexer = layer.lexer.Lexer(
    [(r";", lambda s, r: DELIM),
     (r"&&", lambda s, r: AND),
     (r"\|\|", lambda s, r: OR),
     (r'"([^"\\]|\\.)*"', lambda s, r: s[1:-1].decode('string_escape')),
     (r"'([^'\\]|\\.)*'", lambda s, r: s[1:-1].decode('string_escape')),
     (r"\{[^\s]+\}", lambda s, r: r.get(s[1:-1], s)),
     (r"[^\s;&|]+", lambda s, r: s),
     (r"\s+", None),
        ]).scan

class Command(object):
    def __init__(self, name, func, declspec=None, doc=None):
        self.name = name.lower().strip()
        self.func = func
        self.declspec = declspec
        self.doc = (doc or func.__doc__ or _("Undocumented")).splitlines()[0]

        if declspec:
            self.arity = len(declspec)
        else:
            try: self.arity = func.func_code.co_argcount
            except AttributeError: self.arity = 0
            try: func.im_self
            except AttributeError: pass
            else: self.arity -= 1

    def decl(self, idx):
        if self.arity == -1:
            return _("%s: Unknown arguments.") % self.name
        elif self.arity <= idx:
            return _("%s: No more arguments.") % self.name
        try: args = self.func.func_code.co_varnames
        except AttributeError: args = []
        try: self.func.im_self
        except AttributeError: pass
        else: args = args[1:]

        try: argname = args[idx]
        except IndexError: argname = ""
        try: kindname = self.declspec[idx].__name__
        except (IndexError, TypeError): kindname = ""

        if argname and kindname:
            return _("%s: Argument %d is <%s:%s>.") % (
                self.name, idx + 1, argname, kindname)
        elif argname:
            return _("%s: Argument %d is <%s>.") % (
                self.name, idx + 1, argname)
        else:
            return _("%s: No information about argument %d.") % idx + 1

    def usage(self, moreinfo=None):         
        try: args = self.func.func_code.co_varnames
        except AttributeError: args = []
        try: self.func.im_self
        except AttributeError: pass
        else: args = args[1:]        

        if self.declspec:
            names = []
            for name, kind in zip(args, self.declspec):
                names.append("<%s:%s>" % (name, kind.__name__))
        elif args:
            names = ["<" + arg + ">" for arg in args]
        else:
            names = [_("<arg>")] * self.arity
        if moreinfo:
            return _("Usage: %s (%s)") % (
                self.name + " " + " ".join(names), moreinfo)
        else:
            return _("Usage: %s") % (self.name + " " + " ".join(names))

    def run(self, args):    
        if len(args) > self.arity:
            raise CommandArityError(
                self.usage(_("received %d parameters") % len(args)))

        if self.declspec:
            try: args = [Kind(arg) for (arg, Kind) in zip(args, self.declspec)
                         if arg is not None]
            except Exception, err:
                raise CommandTypeError(self.usage(str(err)))

        try: return self.func(*args)
        except TypeError, err:
            print_e(traceback.format_exc())
            print_d("Command failed", context=self)
            raise CommandArityError(self.usage(str(err)))

class CommandContext(object):
    def __init__(self, name=""):
        super(CommandContext, self).__init__()
        self.name = name
        self.commands = {}

    def command(self,
                func=None, name=None,
                declspec=None, doc=None):
        # Allow @command or @command(...).
        if func is None:
            return lambda func: command(
                func, name=name, declspec=declspec, doc=doc)
        if name is None:
            name = func.func_name
        if not isinstance(name, list):
            name = [name]
        cmd = Command(name[0], func, declspec, doc)
        for n in name:
            self.commands[n.lower()] = cmd
        return func

    def attr_command(self, obj=None, attr=None, **kwargs):
        if obj is None and attr is None:
            return lambda obj, attr: self.attr_command(obj, attr, **kwargs)
        v = getattr(obj, attr) # make sure it exists...
        if v is not None:
            t = type(v)
            if t is bool:
                t = cmd_bool
            kwargs.setdefault("declspec", [t])
        def _cmd(value=None):
            if value is None:
                return getattr(obj, attr)
            else:
                setattr(obj, attr, value)
        kwargs.setdefault("name", attr)
        return command(func=_cmd, **kwargs)

    def bool_command(self, *args, **kwargs):
        kwargs.setdefault("declspec", [cmd_bool])
        return self.attr_command(*args, **kwargs)

    def uncommand(self, keyval):
        """Unregister a command by function or name."""
        try: keys = [keyval.lower()]
        except AttributeError:
            try: keys = map(str.lower, keyval)
            except TypeError:
                keys = [k for k, v in self.commands.iteritems()
                        if v.func == keyval]
        for key in keys:
            try: del(self.commands[key])
            except KeyError: pass

    def run(self, cmdstring, vars={}, active=True):
        cmdstring = cmdstring.strip()
        commands = CommandLexer(cmdstring, vars)[0]
        res = None

        while commands:
            try: idx = commands.index(DELIM)
            except ValueError: idx = len(commands)
            disjuncts, commands = commands[:idx], commands[idx + 1:]

            while disjuncts:
                try: idx = disjuncts.index(OR)
                except ValueError: idx = len(disjuncts)
                conjuncts, disjuncts = disjuncts[:idx], disjuncts[idx + 1:]

                while conjuncts:
                    try: idx = conjuncts.index(AND)
                    except ValueError: idx = len(conjuncts)
                    args, conjuncts = conjuncts[:idx], conjuncts[idx + 1:]
                    
                    cmd = args.pop(0).lower().strip()

                    if cmd[:2] == "++":
                        if active:
                            orig = cmd[2:] + " " + " ".join(map(repr, args))
                            # vars have already been replaced
                            args.insert(0, not self.run(orig))
                            cmd = cmd[2:]
                        else:
                            continue
                    elif cmd[:1] == "+":
                        args.insert(0, active)
                        cmd = cmd[1:]
                    elif cmd[:1] == "-":
                        args.insert(0, not active)
                        cmd = cmd[1:]
                    elif not active:
                        continue

                    try:
                        command = self.commands[cmd]
                    except KeyError:
                        raise CommandNotFoundError(
                            _("Command not found: %s") % cmd)
                    else:                        
                        res = command.run(args)

                    if not res: break
                if res: break
        return res

__default_cmds = CommandContext("Default")
__contexts = [__default_cmds]

command = __default_cmds.command
attr_command = __default_cmds.attr_command
bool_command = __default_cmds.bool_command
uncommand = __default_cmds.uncommand
commands = __default_cmds.commands

def run(cmdstring, print_=None, print_e=print_e, vars={}, active=True):
    """Run a command."""
    for context in __contexts:
        try:
            res = context.run(cmdstring, vars=vars, active=active)
        except CommandNotFoundError:
            # Actually this only means one of the possible commands
            # was not found, but that's basically the same thing.
            pass
        except Exception, err:
            print_e(str(err))
            traceback.print_exc()
            return None
        else:
            if res is not None and print_ is not None:
                print_(isinstance(res, basestring) and res or repr(res))
            return res
    else:
        print_e(_("Command %r not found") % cmdstring)

@command
def echo(string):
    """Print a string."""
    print_(unicode(string))

@command(name=["cmds", "cmdsearch"])
def cmdsearch(term=""):
    """Search for commands matching a string."""
    match = []
    for context in __contexts:
        for key in sorted(context.commands):
            if term in key:
                match.append("%-20s %s" % (key, context.commands[key].doc))
    return "\n".join(match)

@command(name=["?", "help"])
def help(name=None):
    """Show generic command help."""
    if name is None:
        return _("Try ` to close the console, 'cmds <term>', or 'quit'.")
    else:
        name = name.lower()
        for context in __contexts:
            if name in context.commands:
                cmd = context.commands[name]
                return cmd.usage() + ((cmd.doc and "\n" + cmd.doc) or "")
        else:
            return _("Command %r not found, try 'cmds %r>'.") % (name, name)

@command(declspec=[int, str])
def repeat(count, cmd):
    """Repeatedly run a command."""
    for i in xrange(count):
        run(cmd)

def push(context):
    """Add a new command context onto the stack."""
    if context not in __contexts:
        __contexts.append(context)

def pop(context):
    """Remove a command context from the stack."""
    try: __contexts.remove(context)
    except ValueError: pass

def process_argv(argv):
    """Process an executable argument vector as a command sequence.

    This looks for --foo parameters to delimit commands, e.g.
    --foo 1 2 --bar will execute 'foo 1 2' then 'bar'.
    """
    commands = []
    for command in argv:
        if command.startswith("--"):
            if commands:
                run(" ".join(commands))
            commands = [command[2:]]
        elif commands:
            commands.append(command)
    if commands:
        run(" ".join(commands))

def cmd_bool(s):
    """Useful for command declspec coercions."""
    try: s == s.lower()
    except AttributeError: pass
    if not s or s == "false" or s == "off":
        return False
    else:
        try: i = int(s)
        except ValueError: return True
        else: return bool(i)
