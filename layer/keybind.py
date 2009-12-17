# $Id: keybind.py 454 2009-04-30 08:04:32Z joe.wreschnig $

__all__ = ["KeyBind", "KeyBindSet", "init", "quit", "push", "pop"]

import os

import pyglet

import layer.command
import layer.util

from layer.parse import ParseObject
import layer.key

class KeyBind(ParseObject):
    _shelves_ = ["keys", "command"]
    _thawers_ = dict(keys=layer.key.parse)
    _freezers_ = dict(keys=lambda k: layer.key.unparse(*k), command=str)

    def __init__(self, keys, command):
        self.keys = layer.key.parse(keys)
        self.command = command

    keystring = property(lambda self: layer.key.unparse(*self.keys))

    def press(self, state, key, vars={}):
        try: index = self.keys.index(key)
        except ValueError: return False
        
        for key in self.keys:
            if not state[key]:
                return False
        
        layer.command.run(self.command, vars=vars, print_=print_, active=True)
        return True

    def release(self, state, key, vars):
        try: index = self.keys.index(key)
        except ValueError: return False
        
        layer.command.run(self.command, vars=vars, print_=print_, active=False)
        return True

    __repr__ = layer.util.meta_repr_from_list("keys", "keystring", "command")
    
class KeyBindSet(ParseObject):
    load_spec = "*.binds"

    def __init__(self, name, binds=[]):
        self.name = name
        self.binds = binds

    def press(self, state, key, vars={}):
        for bind in self.binds:
            if bind.press(state, key, vars):
                return True

    def release(self, state, key, vars={}):
        handled = False
        for bind in self.binds:
            handled = bind.release(state, key, vars) or handled
        return handled

    def bind(self, keystr, command): 
        """Bind a key string to a command."""
        self.unbind(keystr)
        self.binds.append(KeyBind(keystr, command))
        return True

    def unbind(self, keystr):
        """Unbind a key from a command."""
        keys = layer.key.parse(keystr)
        for bind in self.binds:
            if bind.keys == keys:
                self.binds.remove(bind)
                return True
        return False

    def load_file(self, filename="binds.txt"):
        user_binds = os.path.join(layer.USERDIR, filename)
        try: f = KeyBindSet.load(file(user_binds, "r"))
        except IOError: pass
        else: self.binds.extend(f.binds)

    def save_file(self, filename="binds.txt"):
        user_binds = os.path.join(layer.USERDIR, filename)
        try: f = file(user_binds, "w")
        except IOError:
            pass
        else:
            f.write(self.dump())
            f.close()

__user = KeyBindSet("_User")

__active = [__user]

def __press(input, key, vars={}):
    handled = False
    for set in __active:
        if set.press(input, key, vars):
            return pyglet.event.EVENT_HANDLED
    return pyglet.event.EVENT_UNHANDLED

def __release(input, key, vars={}):
    handled = False
    for set in __active:
        if set.release(input, key, vars):
            handled = True
    if handled:
        return pyglet.event.EVENT_HANDLED
    else:
        return pyglet.event.EVENT_UNHANDLED

__handlers = dict(
    on_input_press=__press,
    on_input_release=__release,
    )

def init(input, first=[False]):
    input.push_handlers(**__handlers)
    if not first[0]:
        __user.load_file()
        first[0] = True

def quit(window):
    window.remove_handlers(**__handlers)

@layer.command.command(name="bind_push")
def push(setname):
    """Push a keybind set on the stack."""
    set = KeyBindSet.Find(setname)
    if set and set not in __active:
        __active.insert(0, set)

@layer.command.command(name="bind_pop")
def pop(setname=None):
    """Pop a keybind set from the stack."""
    if setname is None and len(__active) > 1:
        __active.pop(0)
    else:
        set = KeyBindSet.Find(setname)
        try: __active.remove(set)
        except ValueError: pass

@layer.command.command
def bind(keystr, command):
    """Bind a key string to a command."""
    __user.bind(keystr, command)
    __user.save_file()

@layer.command.command
def unbind(keystr):
    """Unbind a key string from a command."""
    __user.unbind(keystr)
    __user.save_file()

@layer.command.command
def bind_print(term=""):
    """Print active binds"""
    binds = []
    for i, set in enumerate(__active):
        binds.append(_("Set #%d: %s") % (i, set.name))
        for bind in set.binds:
            if term in bind.keystring or term in bind.command:
                binds.append("  %-20s %s" % (bind.keystring, bind.command))
    return "\n".join(binds)
            
