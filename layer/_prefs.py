# $Id: _prefs.py 348 2009-04-13 03:16:41Z joe.wreschnig $

import os
import sys
import traceback

import layer
import layer.parse
import layer.event

class Preferences(object):
    """User preference management.

    This module provides simple key/value pairs as normal Python
    attributes, as well as a way to save them to a file. It also can store
    a default value for each key, in which case the value is not saved to
    the file.

    Example:
      import layer.prefs
      layer.prefs.add('foo', 10)
      layer.prefs.foo # => 10
      layer.prefs.foo = 9
      layer.prefs.foo # => 9, overridin the default
      del(layer.prefs.foo)
      layer.prefs.foo # => 10, back to default
      layer.prefs.save()

    """

    @classmethod
    def IsKeyValid(cls, name):
        """Check if a name is a valid preference key."""
        return (name and name[0].isalpha() and name.replace("_", "").isalnum())

    @classmethod
    def ForceKeyValid(cls, name):
        """Force a name into a valid preference key.

        KeyError is raised if it cannot be one.
        """
        if not cls.IsKeyValid(name):
            raise KeyError("Preferences: %r not a valid name." % name)
        return name.lower()

    def __init__(self, event=layer.event.post, load=layer.parse.load,
                 dump=layer.parse.dump):
        self._defaults = {}
        self._values = {}
        self._event = event
        self._load = load
        self._dump = dump

    def add(self, key, default=None):
        """Set a default value for a preference.

        If a preference has its default value, it is not included in the
        saved preferences. If the key currently does not exist, it is
        set to its default value.
        """
        key = self.ForceKeyValid(key)
        if key in self._defaults:
            raise KeyError("%r: Preference already exists." % key)
        self._defaults[key] = default
        self._event("pref-added", name=key, default=default)

    def __delattr__(self, key):
        """Reset a key to its default value."""
        key = self.ForceKeyValid(key)
        try: self.__setattr__(key, self._defaults[key])
        except KeyError: del(self._values[key])

    def __setattr__(self, key, value):
        """Set a preference value."""
        if not key.startswith("_"):
            key = self.ForceKeyValid(key)
            old_value = self._values.get(key, self._defaults.get(key))
            if value == self._defaults.get(key):
                del(self._values[key])
            else:
                self._values[key] = value
            if value != old_value:
                self._event("pref-changed", name=key,
                            old_value=old_value, new_value=value)
        else:
            return super(Preferences, self).__setattr__(key, value)

    def __getattr__(self, key):
        try:
            key = self.ForceKeyValid(key)
            try: return self._values[key]
            except KeyError:
                return self._defaults[key]
        except KeyError:
            raise AttributeError, key

    def load(self, filename="config.txt"):
        """Load preferences from a file.

        The default is ~/.<program>/config.txt.
        """
        filename = os.path.join(layer.USERDIR, filename)
        try:
            print_d("Loading preferences from %r." % filename)
            fileobj = file(filename, "rU")
        except (OSError, IOError):
            return
        try:
            for key, value in self._load(fileobj).items():
                try: self.__setattr__(key, value)
                except KeyError:
                    print_e("Invalid preference key %r, discarding." % key)
        except Exception:
            print_e(traceback.format_exc())

    def save(self, filename="config.txt"):
        """Save preferences to a file.

        The default is ~/.<program>/config.txt.
        """
        filename = os.path.join(layer.USERDIR, filename)
        try:
            print_d("Saving preferences to %r." % filename)
            fileobj = file(filename, "w")
            fileobj.write(self.write())
            fileobj.close()
        except Exception:
            print_e(traceback.format_exc())

    def write(self):
        return self._dump(self._values)

    def clear(self, all=False):
        self._values.clear()
        if all:
            self._defaults.clear()

layer.event.register("pref-added", kwtemplate=["name", "default"])
layer.event.register(
    "pref-changed", kwtemplate=["name", "old_value", "new_value"])

