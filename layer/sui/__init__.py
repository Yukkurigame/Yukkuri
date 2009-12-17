# $Id: __init__.py 404 2009-04-23 07:32:59Z joe.wreschnig $

"""layer.sui -- Simple UI.

This is a very simple UI library.
"""

import pyglet
import layer
import layer.parse
import layer.sprite
import layer.colors
import layer.command
import layer.window
import layer.event
import layer.expr
import layer.util
import weakref

from layer.sui.font import Font

class ScreenDef(layer.parse.ParseObject):
    """A collection of widgets.

    Attributes:

    mode - 'absolute' or 'scaled'.
      If absolute, widget locations on this screen are cnsidered pixel
      locations. If scaled, locations should be 0 to 100, and are scaled
      to the actual screen size.
    """

    load_spec = "*.uiscreen"

    mode = "scaled"
    width = None
    height = None

    widgets = []

    _shelves_ = ["mode", "width", "height", "widgets"]

class WidgetDef(layer.parse.ParseObject):
    name = None
    overrides = []
    animation = 0
    messages = {}
    _shelves_ = ["overrides", "animation", "messages", "name"]
    _freezers_ = dict(messages=layer.expr.Decompile)
    _thawers_ = dict(messages=layer.expr.Compile)

class WidgetState(layer.parse.ParseObject):
    name = None
    origin = "topleft"
    color = (255, 255, 255)
    color4 = None
    x = 0
    y = 0
    width = 0
    height = 0
    on_enter = []
    on_exit = []
    text = ""
    font = None
    texture = None
    opacity = 255

    _shelves_ = ["name", "origin", "color", "color4", "opacity",
                 "texture", "x", "y", "width", "height",
                 "text", "font",
                 "on_enter", "on_exit"]
    _freezers_ = dict(color=layer.colors.name,
                      color4=lambda l: map(layer.colors.name, l),
                      on_enter=layer.expr.Decompile,
                      on_exit=layer.expr.Decompile,
                      font=Font.Freeze,
                      )
    _thawers_ = dict(color=layer.colors.value,
                     color4=lambda l: map(layer.colors.value, l),
                     on_enter=layer.expr.Compile,
                     on_exit=layer.expr.Compile,
                     font=Font.Thaw,
                     )

    def interp(self, other, p):
        q = 1 - p
        state = WidgetState()
        state.__dict__.update(self.__dict__)
        state.__dict__.update(other.__dict__)
        state.x = self.x * q + other.x * p
        state.y = self.y * q + other.y * p
        state.opacity = self.opacity * q + other.opacity * p
        state.width = self.width * q + other.width * p
        state.height = self.height * q + other.height * p
        state.color = layer.colors.interpolate(self.color, other.color, p)
        if self.color4 and other.color4:
            state.color4 = [layer.colors.interpolate(c1, c2, p)
                            for c1, c2 in zip(self.color4, other.color4)]
        else:
            state.color4 = other.color4 or self.color4
        if self.font and other.font:
            state.font = self.font.interpolate(other.font, p)

        return state

class Widget(object):
    sprite = None
    result = None
    label = None

    # FIXME: Maybe we need a rect type.
    x = 0
    y = 0
    width = 0
    height = 0

    dirty_result = True
    dirty_layout = True
    result = None
    animation = 0

    def __init__(self, dfn, screen):
        self.dfn = dfn
        self.states = set(["base", "last"])
        self._screen = weakref.ref(screen)

    screen = property(lambda self: self._screen())

    def __del__(self):
        # pyget will leak the glyphs and leave them onscreen even if
        # the label dies, so we need to manually delete them.
        if self.label:
            self.label.delete()

    def message(self, message, **kwargs):
        message = message.lower()
        if message in self.dfn.messages:
            try: context = self.screen.context
            except AttributeError: context = None
            self.dfn.messages[message](context=context, self=self, **kwargs)

    def state(self, state, on):
        if on:
            return self.enter(state)
        else:
            return self.exit(state)

    def enter(self, state):
        if state in self.states:
            return False
        else:
            self.states.add(state)
            for override in self.dfn.overrides:
                if override.name == state:
                    if override.on_enter:
                        try: context = self.screen.context
                        except AttributeError: context = None
                        override.on_enter(context=context, self=self)
                    self.dirty_result = True
                    break
            return True
        
    def exit(self, state):
        try: self.states.remove(state)
        except KeyError:
            return False
        else:
            for override in self.dfn.overrides:
                if override.name == state:
                    if override.on_exit:
                        try: context = self.screen.context
                        except AttributeError: context = None
                        override.on_exit(context=context, self=self)
                    self.dirty_result = True
                    break
            return True

    def update(self, dt, window):
        self.message("update")
        if self.result is None or self.dirty_result:
            self.message("result")
            self.create_result()
            self.dirty_result = False
            self.dirty_layout = True
            self.enter("visible")

        if self.dfn.animation > self.animation:
            self.message("animate")
            self.animation += dt
            p = min(1.0, self.animation / self.dfn.animation)
            self.result = self.old_result.interp(self.new_result, p)
            self.dirty_layout = True

        if self.result.text and "%" in self.result.text:
            self.dirty_layout = True

        if self.dirty_layout:
            self.message("layout")
            self.layout(window)
            self.dirty_layout = False

    def layout(self, window):
        screen = self.screen
        if screen is None:
            return
        if screen.dfn.mode == "scaled":
            x = round(self.result.x / screen.scale_x)
            y = round(self.result.y / screen.scale_y)
            width = round(self.result.width / screen.scale_x)
            height = round(self.result.height / screen.scale_y)
        else:
            x = self.result.x
            y = self.result.y
            width = self.result.width
            height = self.result.height

        self.width = width
        self.height = height

        if self.sprite is None or self.sprite.filename != self.result.texture:
            if self.sprite is not None:
                self.sprite.delete()
                self.sprite = None
            if self.result.texture:
                self.sprite = layer.sprite.Sprite(filename=self.result.texture)
        if self.label and self.label.font is not self.result.font:
            self.label.delete()
            self.label = None
        if self.label is None and self.result.font and self.result.text:
            self.label = self.result.font.Label(
                self.result.text,
                group=layer.sprite.batch.Group(0),
                batch=layer.sprite.batch)
            self.label.anchor_x = "center"
            self.label.anchor_y = "center"

        if self.sprite:
            if not self.width:
                self.width = self.sprite.width
            if not self.height:
                self.height = self.sprite.height

        if self.label:
            self.label.text = self.result.text % layer.util.attrdict(
                self.screen.context.as_dict())
            if not self.width:
                self.width = self.label.content_width
            if not self.height:
                self.height = self.label.content_height

        if "left" in self.result.origin:
            self.x = x
        elif "right" in self.result.origin:
            self.x = window.width - (x + self.width)
        else:
            self.x = x + (window.width - self.width) / 2
            
        if "bottom" in self.result.origin:
            self.y = y
        elif "top" in self.result.origin:
            self.y = window.height - (y + self.height)
        else:
            self.y = y + (window.height - self.height) / 2

        if self.sprite:
            self.sprite.position = self.x, self.y
            self.sprite.width = self.width
            self.sprite.height = self.height
            self.sprite.opacity = self.result.opacity
            if self.result.color4:
                self.sprite.color4 = self.result.color4
            else:
                self.sprite.color = self.result.color

        if self.label:
            self.label.x = self.x + self.width / 2
            self.label.y = self.y + self.height / 2

    def create_result(self):
        self.animation = 0

        try:
            self.old_result = self.result
        except AttributeError:
            self.old_result = None
        self.new_result = new_result = WidgetState()

        dfn = self.dfn
        for override in self.dfn.overrides:
            if override.name in self.states:
                new_result.__dict__.update(override.__dict__)

        if self.old_result is None:
            self.animation = self.dfn.animation

        if self.animation >= self.dfn.animation:
            self.result = self.new_result

class Screen(object):
    def __init__(self, window, dfn, context):
        self.window = window
        self.widgets = []
        self.dfn = dfn
        self.context = context
        self._create_widgets()
        self._focused = None

    def __getitem__(self, name):
        name = name.lower()
        for widget in self.widgets:
            if widget.dfn.name.lower() == name:
                return widget

    def _create_widgets(self):
        if self.dfn.width is None and self.dfn.height is None:
            self.scale_x = 1.0 / self.window.width
            self.scale_y = 1.0 / self.window.height
        elif self.dfn.height is not None:
            self.scale_y = float(self.dfn.height) / self.window.height
            self.scale_x = self.scale_y
        elif self.dfn.width is not None:
            self.scale_x = float(self.dfn.width) / self.window.width
            self.scale_y = self.scale_x
        else:
            self.scale_x = self.dfn.width / self.window.width
            self.scale_y = self.dfn.height / self.window.height
        self.widgets = []
        for dfn in self.dfn.widgets:
            widget = Widget(dfn, self)
            self.widgets.append(widget)

    def _get_focused(self):
        if self._focused is not None:
            focused = self._focused()
            if focused is not None:
                return focused
            else:
                self._focused = None

    def focus(self, widget):
        focused = self.focused
        if focused is widget:
            return
        if focused:
            focused.exit("focused")
        if widget:
            self._focused = weakref.ref(widget)
            widget.enter("focused")

    focused = property(_get_focused, focus)

class UIManager(object):
    def __init__(self, window, globals):
        self.window = window
        self.screens = []
        self.task = layer.task.Task(self.update, enabled=True)
        self.context = layer.expr.EvalContext("UIManager", globals)
        self.context["_"] = _
        window.push_handlers(self)

    def show(self, screen):
        for scr in self.screens:
            if scr.dfn is screen: break
        else:
            self.screens.append(Screen(self.window, screen, self.context))

    def hide(self, screen):
        for scr in list(self.screens):
            if scr.dfn is screen:
                self.screens.remove(scr)

    def on_mouse_motion(self, x, y, dx, dy):
        for screen in self.screens:
            for widget in screen.widgets:
                over = (widget.x <= x < widget.x + widget.width and
                        widget.y <= y < widget.y + widget.height)
                widget.state('mouseover', over)

    def on_mouse_press(self, x, y, button, modifiers):
        for screen in self.screens:
            for widget in screen.widgets:
                over = (widget.x <= x < widget.x + widget.width and
                        widget.y <= y < widget.y + widget.height)
                widget.state('mousepress', over)

    def on_mouse_release(self, x, y, button, modifiers):
        for screen in self.screens:
            for widget in screen.widgets:
                over = (widget.x <= x < widget.x + widget.width and
                        widget.y <= y < widget.y + widget.height)
                if widget.state('mousepress', False) and over:
                    widget.message("activate")

    def update(self, dt):
        for screen in self.screens:
            # If we're in debug mode, be prepared for reloads.
            if layer.DEBUG:
                if len(screen.widgets) == len(screen.dfn.widgets):
                    for w, dfn in zip(screen.widgets, screen.dfn.widgets):
                        if w.dfn is not dfn:
                            screen._create_widgets()
                            break
                else:
                    screen._create_widgets()
            for widget in screen.widgets:
                widget.update(dt, self.window)

manager = None

show = lambda *args, **kwargs: manager.show(*args, **kwargs)
hide = lambda *args, **kwargs: manager.hide(*args, **kwargs)

def init(window, globals={}):
    global manager
    manager = UIManager(window, globals)

@layer.command.command(declspec=[ScreenDef.Find])
def ui_show(screen):
    """Show a UI screen."""
    if screen is None:
        print_e("ScreenDef not found.")
        return
    else:
        manager.show(screen)

@layer.command.command(declspec=[ScreenDef.Find])
def ui_hide(screen):
    """Hide a UI screen."""
    if screen is None:
        print_e("ScreenDef not found.")
    else:
        manager.hide(screen)
