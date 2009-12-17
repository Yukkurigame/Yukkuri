# $Id: event.py 382 2009-04-19 08:50:54Z joe.wreschnig $

"""Global event manager.

A global event manager for game systems. This differs from the pyglet
event manager in a few key conceptual ways. There's only ever one of
these event managers, and the list of events is not fixed by type.
Consider the pyglet event framework as fixed systems sending fixed
events to arbitrary systems. This is arbitrary systems sending
arbitrary events to arbitrary systems;

The intention is coarser-grained event senders, e.g. the 'audio
system' will send the event, rather than an individual player;
anything interested in any audio events can connect to that, rather
than needing to track down each individual audio player to connect to.

It also has some features the pyglet event handler does not. Notably:
 * Early event argument count enforcement.
 * Various debug modes.
 * Queued versus immediate events.

"""

# Although this is not meant to be used like normal pyglet event
# dispatchers, there's no reason it couldn't be implemented as an
# instance of one. However, that makes it a kind of debugging black
# hole, and an event handler is not very complicated. So it's custom
# for now.

import layer.command
from layer.parse import ParseObject

__all__ = ["Error", "register", "tick", "listen", "unlisten", "fire", "post"]

class Error(Exception): pass

class EventManager(object):
    def __init__(self):
        self._argcheck = {}
        self._listeners = {}
        self._queue = []
        self._id = 0
        self.debug = False

    def _log(self, string):
        if self.debug:
            print_d(string, context=self)
        return True

    def __check_event_args(self, event, args, kwargs):
        event = event.lower()
        try: argcount, kwtemplate = self._argcheck[event]
        except KeyError:
            raise Error("Event %r does not exist." % event)
        if argcount is not None and len(args) != argcount:
            raise TypeError("Event arg mismatch: %s: %r != %r" %(
                event, argcount, len(args)))
        if kwtemplate is not None:
            for kw in kwargs:
                if kw not in kwtemplate:
                    raise TypeError(
                        "Event mismatch: %s: %r not in kw template." %(
                        event, kw))
            for kw in kwtemplate:
                if kw not in kwargs:
                    raise TypeError(
                        "Event mismatch: %s: %r not in kwargs." %(
                            event, kw))
        return True

    def _dispatch(self, event, args, kwargs):
        listeners = self._listeners.get(event, [])
        self._log("Dispatching: %r." % event)
        for id, func, extra_args, extra_kwargs in listeners:
            l_args = args + extra_args
            extra_kwargs.update(kwargs)
            func(*l_args, **extra_kwargs)

    def tick(self, dt=None):
        """Run all queued events."""
        for event, args, kwargs in self._queue:
            self._dispatch(event, args, kwargs)
        self._queue = []

    def fire(self, event, *args, **kwargs):
        """Run an event immediately.

        If you know your event is safe to run immediately and you want
        low latency, you can use this function instead of post.
        """
        event = event.lower()
        assert(self.__check_event_args(event, args, kwargs))
        # Force events to now so nothing arrives out of order.
        self.tick()
        self._dispatch(event, args, kwargs)

    def cancel(self, event):
        """Remove all events of the given type from the queue."""
        event = event.lower()
        self._log("Cancelling: %r." % event)
        self._queue = [ev for ev in self._queue if ev[0] != event]

    def post(self, event, *args, **kwargs):
        """Post an event.

        This should be the default behavior to trigger an event; it
        actually queues it for the next tick, but since events have a
        tendency to cause listeners to change, this is the safest way
        to do it.
        """
        event = event.lower()
        assert(self.__check_event_args(event, args, kwargs))
        self._log("Posting: %r." % event)
        self._queue.append((event, args, kwargs))

    def listen(self, event, func, *extra_args, **extra_kwargs):
        """Listen for the event and call the function when triggered.

        A list of additional arguments can be given.
        """
        event = event.lower()
        self._id += 1
        state = (self._id, func, extra_args, extra_kwargs)
        self._listeners[event].append(state)
        return self._id

    def unlisten(self, id=None, event=None, func=None):
        """Stop listening for an event.

        Either an event name and function can be given, or a numeric
        ID returned from the listen call.
        """
        if id is not None and (event or func) is not None:
            raise ValueError("Unlisten takes an id or an event/func.")

        elif id is not None:
            for listeners in self._listeners.values():
                for i, l in enumerate(listeners):
                    if l[0] == id:
                        del(listeners[i])
                        return True
            return False

        elif event is not None:
            event = event.lower()
            try: listeners = self._listeners[event]
            except KeyError:
                return False
            else:
                to_remove = []
                for i, l in enumerate(listeners):
                    if l[1] == func:
                        to_remove.append(i)
                for i in reversed(to_remove):
                    del(listeners[i])
                return bool(to_remove)

        else:
            any = False
            for listeners in self._listeners.values():
                to_remove = []
                for i, l in enumerate(listeners):
                    if l[1] == func:
                        to_remove.append(i)
                for i in reversed(to_remove):
                    del(listeners[i])
                any = any or bool(to_remove)
            return any

    def register(self, event, argcount=None, kwtemplate=None):
        """Register a new event.

        Optionally an argument count and a list of valid keyword
        arguments can be supplied, for listen-time checking.
        """
        event = event.lower()
        if event in self._listeners:
            raise Error("Duplicate event name %r." % event)
        elif not event.replace('-', '').isalpha():
            raise Error("Invalid event name %r." % event)
        self._listeners[event] = []
        self._argcheck[event] = (argcount, kwtemplate)

    def unregister(self, event):
        """Unregister an event (and all listeners) by name.

        This is intended for testing. Don't use it in normal programs.
        """
        event = event.lower()
        try:
            del(self._listeners[event])
            del(self._argcheck[event])
        except KeyError:
            return False
        else:
            return True

_manager = EventManager()
register = _manager.register
tick = _manager.tick
listen = _manager.listen
unlisten = _manager.unlisten
fire = _manager.fire
post = _manager.post
unregister = _manager.unregister

class EventSpec(ParseObject):
    parse_tag = "!Event"
    load_spec = "*.events"
    argcount = None
    kwargs = None
    _shelves_ = ["name", "argcount", "kwargs"]
    _thawers_ = dict(name=str.lower)

    @classmethod
    def Load(cls):
        super(EventSpec, cls).Load()
        for ev in cls.Instances.values():            
            if ev.name in _manager._listeners:
                if _manager._argcheck[ev.name] != (ev.argcount, ev.kwargs):
                    print_e("%s: Reloading event with new specification." %(
                        ev.name))
            else:
                _manager.register(ev.name, ev.argcount, ev.kwargs)

layer.command.bool_command(_manager, "debug", name="event_debug",
                           doc="Event debug logging.")

layer.command.command(_manager.register, name="event_register",
                      declspec=[str, int, lambda s: s.split()])

