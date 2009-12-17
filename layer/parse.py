# $Id: parse.py 425 2009-04-27 02:04:41Z joe.wreschnig $

"""layer.parse -- Object serialization support

This module provides several classes for import and export in common
formats used by Layer. It provides a class, ParseObject, which has a
number of interfaces for efficient and convenient serialization and
instantiation.

Currently the format used is YAML; future developments may include
Pickle formats, depending on optimization concerns.
"""

import re

import yaml

import layer.load

__all__ = ["load", "dump", "ParseObject", "ParseScalar"]

try:
    from yaml import CLoader as _Loader
    from yaml import CDumper as _Dumper
except ImportError:
    print_w(_("LibYAML implementation not found, using Python-native."))
    from yaml import Loader as _Loader
    from yaml import Dumper as _Dumper

def load(data):
    """Load data from a string/file."""
    return yaml.load(data, Loader=_Loader)

def dump(data):
    """Write data to a string/file."""
    return yaml.dump(data, Dumper=_Dumper, width=72, indent=2)

class _ParseScalarMetaclass(type):
    """The basic metaclass used by ParseScalar.

    It is used to set up class-level variables, as well as register
    the class with the internal parsers.
    """

    __slots__ = ()

    def __new__(cls, name, bases, kwds):
        kwds.setdefault("parse_tag", "!" + ("." + name).rsplit(".", 1)[1])
        return type.__new__(cls, name, bases, kwds)

    def __init__(cls, name, bases, kwds):
        super(_ParseScalarMetaclass, cls).__init__(name, bases, kwds)

        cls._Loader.add_constructor(cls.parse_tag, cls._from_yaml)
        cls._Dumper.add_representer(cls, cls._to_yaml)

        try: pattern = re.compile(cls.parse_pattern)
        except AttributeError: pass
        else: cls._Loader.add_implicit_resolver(cls.parse_tag, pattern, None)

class ParseScalar(object):
    """A parseable tagged scalar type.

    This does for simple types (e.g. strings and numbers) what
    ParseObject does for mapped types (e.g. objects). It lets you
    specify a tag to prefix them with, which is used to construct
    them.

    Unlike ParseObject, ParseScalars are not forced to have a name
    attribute, and do not maintain an instance dictionary.

    Scalar types can ibe implemented at one of two levels. If they are
    a mutable type, they can implement __getstate__ and __setstate__,
    much like ParseObjects. However, these must return a string.

    If the type is immutable, and so needs to be parsed at
    instantiation time rather than using __setstate__ on an existing
    instance, implement the _FromScalar_ class method, and the
    _toscalar_ instance method. _FromScalar_ takes a string and
    returns an instance; _toscalar_ takes an instance and returns a
    string.

    If neither __setstate__ nor _FromScalar_ is implemented, it is
    assumed the class implements a default constructor that accepts
    the output of _toscalar_. If neither __getstate__ nor _toscalar_
    is implemented, it is assumed that you want the result of repr.

    By default, the unqualified name of the class is used to tag the
    type in the serialized file. If you don't like this (e.g. you need
    a more qualified name or you renamed a class but want to read the
    same files), you can specify the parse_tag attribute:
        parse_tag = 'CustomName'

    If you want to automatically parse scalars matching a certain
    pattern as a particular type, use the parse_pattern regular
    expression:
        parse_pattern = r"^\d+%$"
    Now, any token like 23% will automatically use this class.
    """

    __metaclass__ = _ParseScalarMetaclass

    _Loader = _Loader
    _Dumper = _Dumper

    @classmethod
    def _FromScalar_(cls, s):
        try: cls.__setstate__
        except AttributeError:
            obj = cls(s)
        else:
            obj = cls.__new__(cls)
            obj.__setstate__(s)
        return obj

    def _toscalar_(self):
        try: getstate = self.__getstate__
        except AttributeError: return repr(self)
        else: return getstate()

    @classmethod
    def _from_yaml(cls, loader, node):
        return cls._FromScalar_(loader.construct_scalar(node))

    @classmethod
    def _to_yaml(cls, dumper, data):
        return dumper.represent_scalar(data.parse_tag, data._toscalar_())

class percentage(ParseScalar, float):
    parse_pattern = r"^[-+]?\d+(\.\d+)?%$"

    @classmethod
    def _FromScalar_(cls, s):
        return cls(float(s[:-1]) / 100.0)
        f = float(loader.construct_scalar(node)[:-1])
        return cls.__new__(cls, f / 100.0)

    def _toscalar_(self):
        return "%g%%" % (self * 100)

class _ParseObjectMetaclass(type):
    """The basic metaclass used by ParseObject.

    It is used to set up class-level variables, as well as register
    the class with the internal parsers.
    """

    __slots__ = ()

    def __new__(cls, name, bases, kwds):
        if "load_spec" in kwds:
            kwds.setdefault("Instances", {})
        kwds.setdefault("parse_tag", "!" + ("." + name).rsplit(".", 1)[1])
        return type.__new__(cls, name, bases, kwds)

    def __init__(cls, name, bases, kwds):
        super(_ParseObjectMetaclass, cls).__init__(name, bases, kwds)
        cls._Loader.add_constructor(cls.parse_tag, cls._from_yaml)
        cls._Dumper.add_representer(cls, cls._to_yaml)

        try: cls.load_spec
        except AttributeError: pass
        else: cls.LoadableKinds.append(cls)

        try: shelves = cls._shelves_
        except AttributeError: pass
        else:
            if "name" not in cls._shelves_:
                cls._shelves_.append("name")

        try: thawers = cls._thawers_
        except AttributeError: thawers = {}
        for key, thawer in thawers.items():
            if thawer == "auto":
                try:
                    default = getattr(cls, key)
                except AttributeError:
                    raise TypeError(
                        "Thawer for %r is auto but has no default" % key)
                else:
                    try: thawers[key] = type(default)._FromScalar_
                    except AttributeError: type(default)

        try: freezers = cls._freezers_
        except AttributeError: freezers = {}
        for key, freezer in freezers.items():
            if freezer == "auto":
                try:
                    default = getattr(cls, key)
                except AttributeError:
                    raise TypeError(
                        "Freezer for %r is auto but has no default" % key)
                else:
                    try:
                        freezers[key] = type(default)._toscalar_
                    except AttributeError:
                        try:
                            freezers[key] = type(default).__getstate__
                        except AttributeError:
                            del(freezers[key])
                
class ParseObject(object):
    """A class that makes your object parseable.

    Inheriting from this class allows you to pass it to parse.dump
    or parse.load and get stringed/de-stringed versions of it. By
    default you don't have to do anything to make it work right.

    Automatic Loading:
    ------------------
    To make your class read files at startup, provide a load_spec
    attribute that is a glob to match against the desired files, e.g.
        load_spec = '*.keybind'

    This attribute causes the class to be registered in
    ParseObject.LoadableKinds, and the Load method of each such class
    is called at startup and, if matching, during reload.

    All ParseObjects that have a load_spec will need a 'name'
    attribute, or certain things like reloading and the Find class
    method will fail.

    Custom Parsing:
    ---------------
    There are a few class-level attributes you can set to change the
    serialization process to fit your needs.

    _shelves_
        Normally, all attributes of the object not beginning with _
        are serialized with the object. If _shelves_ is defined,
        exactly the attributes in that list are serialized. The 'name'
        attribute is automatically added to _shelves_ if it exists.

    _freezers_
        Normally, all values of attributes in the object are passed
        onto the serializer as-is. If you need to massage the data,
        use _freezers_ to map keys to custom data massagers. The
        value of each _freezers_ entry is a callable that takes
        either a single argument of the given value, or three
        arguments of the object, the key, and the value. The value
        returned from this callable is what's actually serialized.

        _freezers_ does not change the original object, unless you
        explicitly write a 3-ary method which does.

        Example:
            # force a to be an int, and b to actually be o._b.
            _freezers_ = dict(a=int, b=lambda o, k, v: getattr(o, "_" + k))

    _thawers_
        This is the opposite of _freezers_. When the file is read
        back in, the values are fed through an analogous dictionary
        before they are set on the object.

        Example:
            # force obj.c to be a lowercase string.
            _thawers_ = dict(c=str.lower)

    If you need even more custom parsing, you can implement the
    __getstate__ and __setstate__ methods, which return and receive a
    dict representing the object's state.

    By default, the unqualified name of the class is used to tag the
    type in the serialized file. If you don't like this (e.g. you need
    a more qualified name or you renamed a class but want to read the
    same files), you can specify the parse_tag attribute:
        parse_tag = 'CustomName'
    """

    __metaclass__ = _ParseObjectMetaclass

    _Loader = _Loader
    _Dumper = _Dumper

    _parse_flow_style = False

    LoadableKinds = []

    @classmethod
    def _from_yaml(cls, loader, node):
        return loader.construct_yaml_object(node, cls)

    @classmethod
    def _to_yaml(cls, dumper, data):
        return dumper.represent_yaml_object(
            cls.parse_tag, data, cls, flow_style=cls._parse_flow_style)

    @classmethod
    def load(cls, data):
        """Create an object from data, fail if it's the wrong type."""
        result = yaml.load(data, Loader=cls._Loader)
        if not isinstance(result, cls):
            raise TypeError(
                "Loaded data type %r != loader type %r" % (type(data), cls))
        return result

    @classmethod
    def load_list(cls, data):
        """Create a list of objects from data, fail if it's the wrong type.

        If data describes a single object, a list containing it is
        returned.
        """
        result = load(data)
        if isinstance(result, list) and result:
            types = map(type, result)
            if not (max(types) == min(types) == cls):
                raise TypeError(
                    "Loaded list types %r, %r != loader type %r" %
                    (max(types), min(types), cls))
        elif not isinstance(result, cls):
            raise TypeError(
                "Loaded data type %r != loader type %r" % (type(data), cls))
        elif not isinstance(result, list):
            result = [result]
        return result

    def __getstate__(self):
        try: shelves = self._shelves_
        except AttributeError:
            shelves = [k for k in self.__dict__ if not k.startswith("_")]
        d = dict([(k, self.__dict__[k]) for k in shelves
                  if self.__dict__.get(k) is not None])

        try: freezers = self._freezers_
        except AttributeError: freezers = {}
        for key, freezer in freezers.items():
            try:
                d[key] = freezer(d[key])
            except TypeError:
                d[key] = freezer(self, key, d[key])
            except KeyError:
                pass

        return d

    def __setstate__(self, d):
        try: shelves = self._shelves_
        except AttributeError: pass
        else:
            for key in d:
                if key not in shelves:
                    raise ValueError("Invalid field %r." % key)

        try: thawers = self._thawers_
        except AttributeError: thawers = {}
        for key, thawer in thawers.items():
            try:
                d[key] = thawer(d[key])
            except TypeError:
                d[key] = thawer(self, key, d[key])
            except KeyError:
                pass
        self.__dict__.update(d)

    def dump(self):
        """Create a string from this object."""
        return yaml.dump(self, Dumper=self._Dumper)

    def _register_instance(self, name=None):
        """Call this to register an instance for Find."""
        if name is None:
            name = self.name
        name = name.lower()
        self.__filename = None
        if name in type(self).Instances:
            raise ValueError("%r already exists in %s instances" %(
                    name, type(self).__name__))
        type(self).Instances[name] = self

    @classmethod
    def load_match(cls, pattern, dict=None, errors=None):
        """Load, into dict, objects from files matching the pattern."""
        loaded = []
        for filename in layer.load.glob(pattern):
            try:
                objs = cls.load_list(layer.load.file(filename, "rU"))
            except Exception, err:
                print_e("Unable to load %r: %s" % (filename, str(err)))
                try: errors.append(err)
                except AttributeError: pass
            else:
                for obj in objs:
                    loaded.append(obj)
                    try: obj.__filename = filename
                    except AttributeError: pass
        for obj in loaded:
            try:
                old_obj = dict[obj.name.lower()]
            except KeyError:
                dict[obj.name.lower()] = obj
            except (AttributeError, TypeError):
                pass
            else:
                old_obj.__setstate__(obj.__getstate__())
                obj = old_obj
        return loaded

    @classmethod
    def Find(cls, name, default=None):
        """Find an instances by case-insensitive name."""        
        return cls.Instances.get(name.lower(), default)

    @classmethod
    def FindAll(cls, param, default=None):
        ret = {}                
        for inst in cls.Instances.keys():            
            prm = cls.Instances.get(inst.lower(), default)
            if hasattr(prm, param):
                prm = getattr(prm, param)            
                ret[inst] = prm        
        return ret

    @classmethod
    def Load(cls):
        """Load all objects of this type into its dictionary."""
        errors = []
        print_d("Loading %s... " % cls.__name__)
        loaded = cls.load_match(
            pattern=cls.load_spec, dict=cls.Instances, errors=errors)
        print_d("\tdone (%d loaded, %d errors)" % (len(loaded), len(errors)))
        print_("%s loading done (%d loaded, %d errors)" % (cls.__name__, len(loaded), len(errors)))
        return errors

    @classmethod 
    def Thaw(cls, obj):
        """Thaw an instance or name.

        This allows you to specify a loaded field's value as the name
        of something else in the instance dictionary, or as an
        inline-defined object.

        If you use this, and changed the thawed value, be aware you
        may be changing the instance in the dictionary.
        """
        if isinstance(obj, cls):
            return obj
        else:
            return cls.Find(obj, "default")

    @classmethod 
    def Freeze(cls, obj):
        """Freeze an instance.

        If the instance is in the dictionary, a reference to the
        version in the dictionary is written out. Otherwise, it is
        written out inline.
        """
        if obj is cls.Instances.get(obj.name):
            return obj.name
        else:
            return obj
