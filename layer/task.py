# $Id: task.py 461 2009-05-01 05:05:03Z joe.wreschnig $

import pyglet
import layer.command
import layer.tracker

class Task(layer.tracker.InstanceTracker):
    """Register a function to run periodically.

    The function first takes a float, which is the time since last
    it was called by the task, and any other arguments specified.

    Tasks can be run with a variety of different timing parameters via
    keyword arguments. All non-keyword arguments and other keyword
    arguments are passed to the function.

      soft - bool
        Soft timing does not happen in step with other timing, but still
        happens on accurate intervals. Timing is soft by default.

      period - float
        The approximate number of seconds between calls.

      lower_bound - float
        The minimum reported time between calls. Normally if the function
        is called earlier than `period' since the last time it simply
        is called with a smaller dt; if the dt is less than lower_bound
        it will call the function later instead.

      upper_bound - float
        The maximum reported time between calls. If there is no upper
        bound it is the same as the period, i.e. dt will be period
        or less always. This does not ensure you will actually get
        called every upper_bound seconds! It just means your simulated
        step will never be greater than this value, and so you may
        get multiple calls in a row.

      enabled - bool
        Tasks are disabled by default; pass True to start the task
        immediately upon creating it.

      stoppable - bool
        If Task is stoppable, a function can return True to make it
        disable itself. By default a function's return value is ignored.

      pausable - bool
        If a Task is pausable, it is affected by the class-level
        Task.Paused variable. Not all tasks should be pausable (the UI
        task to unpause the game, for example), but tasks are pausable
        by default.

      scale - float
        This task's time will be scaled by the given float, as well
        as the global Scale.

    Class Attributes:
      Scale - float
        Global time scale; all tasks are multiplied by this.

      Paused - bool
        If true, pausable tasks are paused.
        
    """

    Scale = 1.0

    Paused = False

    __slots__ = ["_enabled", "_soft", "_period", "_lower_bound",
                 "_upper_bound", "_func", "_args", "_kwargs", "_scalable",
                 "_accumulate", "_stoppable", "_pausable", "scale",
                 "_paused"]

    def __init__(self, func, *args, **kwargs):
        self._register_instance()
        self._func = func
        self._args = args
        self._kwargs = kwargs
        self._enabled = False
        self._paused = False
        self._stoppable = kwargs.pop("stoppable", False)
        self._pausable = kwargs.pop("pausable", True)
        self._scalable = kwargs.pop("scalable", True)
        self.set_period(kwargs.pop("period", None),
                        kwargs.pop("soft", True),
                        kwargs.pop("upper_bound", None),
                        kwargs.pop("lower_bound", None))
        self.scale = kwargs.pop("scale", 1.0)
        if kwargs.pop("enabled", False):
            self.enable()

    def set_period(self, period, soft=True, upper_bound=None,
                   lower_bound=None):
        """Set the period and bounds of a task timer.

        If the task is running, it is stopped and started again with
        the new parameters.
        """

        if self._enabled:
            enabled = True
            self.disable()
        else:
            enabled = False
        if period is None:
            if lower_bound is not None:
                period = lower_bound
            elif upper_bound is not None:
                period = upper_bound
        if period is None:
            period = 0
        if lower_bound is None:
            lower_bound = 0
        if upper_bound is None:
            upper_bound = period
        self._period = period
        self._lower_bound = lower_bound
        self._upper_bound = upper_bound
        self._soft = soft
        if enabled:
            self.enable()

    @property
    def total_scale(self):
        return self.scale * (self._scalable and self.Scale or 1)

    def _get_paused(self):
        return self._paused or (self._pausable and self.Paused)
    def _set_paused(self, p):
        self._paused = p
    paused = property(_get_paused, _set_paused)

    def _run(self, dt):
        if self.paused:
            return
        dt *= self.total_scale
        if not dt:
            return
        if (self._period == 0
            or (self._upper_bound == 0 and self._lower_bound == 0)):
            ret = self._func(dt, *self._args, **self._kwargs)
            if self._stoppable and ret:
                self.disable()
            return

        acc = self._accumulate + dt
        while self._upper_bound and acc >= self._upper_bound:
            ret = self._func(self._upper_bound, *self._args, **self._kwargs)
            if self._stoppable and ret:
                self.disable()
                return
            acc -= self._upper_bound

        if acc and acc >= self._lower_bound:
            ret = self._func(acc, *self._args, **self._kwargs)
            if self._stoppable and ret:
                self.disable()
                return
            acc = 0
        self._accumulate = acc

    def enable(self):
        """Start the task (do nothing if running already)."""
        if self._enabled:
            return
        self._accumulate = 0
        self._enabled = True
        if self._period == 0:
            pyglet.clock.schedule(self._run)
        elif self._soft:
            pyglet.clock.schedule_interval_soft(self._run, self._period)
        else:
            pyglet.clock.schedule_interval(self._run, self._period)

    def disable(self):
        """Stop the task (do nothing if already stopped)."""
        if not self._enabled:
            return
        self._enabled = False
        pyglet.clock.unschedule(self._run)

    from layer.util import repr_from_slots as __repr__

def schedule(*args, **kwargs):
    """Schedule a task to run.

    The arguments are the same as the Task constructor but enabled by
    default.
    """
    kwargs.setdefault("enabled", True)
    return Task(*args, **kwargs)
    
layer.command.attr_command(Task, "Scale", name="timescale")
layer.command.bool_command(Task, "Paused", name="pause")

@layer.command.command(name="task_dump")
def dump():
    """Return a list of all Tasks currently alive."""
    return "\n".join(map(repr, Task.Instances.values()))
    
    
