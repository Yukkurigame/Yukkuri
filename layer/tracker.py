# $Id: tracker.py 401 2009-04-23 07:07:40Z joe.wreschnig $

import gc
import weakref

__all__ = ["InstanceTracker"]

class _InstanceTrackerMeta(type):
    def __init__(cls, name, bases, kwds):
        super(_InstanceTrackerMeta, cls).__init__(name, bases, kwds)
        cls.Instances = weakref.WeakValueDictionary()

class InstanceTracker(object):
    """Allow finding an insotance of a class by name.

    This uses weak references, so the instance does not need to
    manually remove itself.

    InstanceTracker is not compatible with ParseObject; you can't
    subclass both. However, they provide very similar instances, so if
    you think you need both, you should just use ParseObject only.
    """

    __metaclass__ = _InstanceTrackerMeta

    def _register_instance(self, name=None):
        """Call this to register an instance for Find."""
        if name is None:
            try: name = self.name
            except AttributeError: name = str(id(self))
        name = name.lower()
        if name in self.Instances:
            raise ValueError(
                "%r already exists in %s instances: %r, referrers are %r" %(
                    name, type(self).__name__, self.Instances[name],
                    gc.get_referrers(self.Instances[name])))
        self.Instances[name] = self

    @classmethod
    def Find(cls, name, default=None):
        """Find an instances by name."""
        return cls.Instances.get(name.lower(), default)
