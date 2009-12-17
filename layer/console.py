# $Id: console.py 309 2009-03-28 07:47:56Z piman $

import pyglet.text

import layer.command
import layer.output
import layer.colors
import layer.key

from layer.sprite import Batch

__all__ = ["Console"]

class StringHistory(list):
    """A list of unique strings ordered by recency of use"""

    __index = -1
    __MAX_SIZE = 50

    def up(self):
        """Return the previous item in the history"""
        self.__index = min(len(self) - 1, self.__index + 1)        
        try: return self[self.__index]
        except IndexError: return ""

    def down(self):
        """Return the next item in the history"""
        self.__index = min(len(self) - 1, max(0, self.__index - 1))
        try: return self[self.__index]
        except IndexError: return ""

    def add(self, string):
        """Add a new item to the history and return it"""
        string = string.strip()
        try: self.pop(self.index(string))
        except ValueError: pass
        if string:
            self.insert(0, string)
        while len(self) > self.__MAX_SIZE:
            self.pop()
        self.__index = -1
        return string

class Completer(object):
    def __init__(self, base, options):
        blower = base.lower()
        self.options = [o for o in options if o.lower().startswith(blower)]
        self.options.sort()
        self.base = base
        if not self.options:
            self.options = [self.base]
        self.suggestion = self.options[0]
        self.single = len(self.options) == 1

    def next(self):
        suggestion = self.suggestion
        try: idx = self.options.index(self.suggestion)
        except ValueError: idx = -1
        try: self.suggestion = self.options[idx + 1]
        except IndexError: self.suggestion = self.options[0]
        return suggestion

class Console(object):
    """Simple graphical console

    An in-game debugging console. It accepts commands and relays
    their return values. It has a basic command history.
    """

    _Batch = Batch(z=100000)
    _BoxGroup = _Batch.Group(0)
    _TextGroup = _Batch.Group(1)

    def __init__(self, window):
        self.visible = False
        self._completer = None
        self.history = StringHistory()
        self._caret_pos = 0

        self._window = window
        self._window.push_handlers(
            on_resize=self._on_resize, on_key_press=self._on_key_press)

        self._entry = pyglet.text.document.UnformattedDocument("")
        self._entry.set_style(
            0, 0,
            dict(font_name="Layer Monospace", font_size=8,
                 color=layer.colors.value("ConsoleEntry", 0xFF)))

        self._log = pyglet.text.document.UnformattedDocument("")
        self._log.set_style(
            0, 0,
            dict(font_name="Layer Monospace", font_size=8,
                 color=layer.colors.value("ConsoleLog", 0xFF)))

        self._box = layer.sprite.Sprite(
            filename="white", batch=self._Batch, group=self._BoxGroup)
        self._box.color = layer.colors.value("ConsoleBackground")
        self._box.opacity = 192

        self._entry_layout = pyglet.text.layout.IncrementalTextLayout(
            self._entry, window.width, 0, batch=self._Batch,
            group=self._TextGroup)
        self._log_layout = pyglet.text.layout.TextLayout(
            self._log, window.width, 0, batch=self._Batch,
            group=self._TextGroup)
        self._log_layout.multiline = True

        self._caret = pyglet.text.caret.Caret(
            self._entry_layout, color=layer.colors.value("ConsoleCaret"))

    def _append_to_log(self, string):
        text = self._log.text.splitlines()
        text.extend(string.splitlines())
        self._log.text = "\n".join(text[-20:])
        self._recalc_heights()

    def _recalc_widths(self, padding=10):
        if self.visible:
            # delay expensive updates while we reset size/position
            self._entry_layout.begin_update()
            self._log_layout.begin_update()

            self._entry_layout.width = self._window.width - padding
            self._entry_layout.x = padding // 2
            self._log_layout.width = self._window.width - padding
            self._log_layout.x = padding // 2
            self._log_layout.width = self._window.width
            self._box.x = self._entry_layout.x
            self._box.width = self._entry_layout.width

            self._entry_layout.end_update()
            self._log_layout.end_update()

    def _recalc_heights(self, max_entry=20):
        if self.visible:
            # delay expensive updates while we reset size/position
            self._entry_layout.begin_update()
            self._log_layout.begin_update()

            entry_height = max(self._entry_layout.content_height, max_entry)
            self._entry_layout.height = entry_height
            self._entry_layout.y = self._window.height - entry_height

            log_height = self._log_layout.content_height
            total_height = entry_height + log_height
            self._log_layout.height = log_height
            self._log_layout.y = self._window.height - total_height
                
            self._box.y = self._log_layout.y
            self._box.height = total_height

            self._entry_layout.end_update()
            self._log_layout.end_update()

    def _on_resize(self, width, height):
        self._recalc_widths()
        self._recalc_heights()

    def _on_text(self, text):
        self._completer = None

        # Ignore control characers and `.
        if min(text) < ' ' or text == "`" or text == "/":
            return pyglet.event.EVENT_HANDLED
        # Disallow spaces at the start.
        elif not text.strip() and self._caret.position == 0:
            return pyglet.event.EVENT_HANDLED

    def _on_text_motion(self, motion):
        max_hist = len(self.history) - 1
        if motion == layer.key.MOTION_UP:
            self._entry.text = self.history.up()
            self._caret.position = len(self._entry.text)
            return pyglet.event.EVENT_HANDLED
        elif motion == layer.key.MOTION_DOWN:
            self._entry.text = self.history.down()
            self._caret.position = len(self._entry.text)
            return pyglet.event.EVENT_HANDLED

    def _tab_complete(self):
        # text[pos] is the character after the caret.
        pos = self._caret.position

        # If we don't have anything, print usage information.
        if pos == 0:
            print_(_("Type 'cmds <term>' for a command list."))
            return

        # If we're over whitespace, jump to the next word.
        text = self._entry.text
        if pos < len(text) and text[pos - 1].isspace():
            while pos < len(text) and text[pos].isspace():
                pos += 1
            while pos < len(text) and not text[pos].isspace():
                pos += 1
            self._caret.position = pos
            return

        pre = self._entry.text[:pos]
        post = self._entry.text[pos:]
        pre_parts = pre.split()
        post_parts = post.split()

        # cut off anything after the caret before the next space.
        if post_parts and post_parts[0][0]:
            post_parts.pop(0)

        if len(pre_parts) == 1:
            if not self._completer:
                self._completer = Completer(
                    pre_parts[0], layer.command.commands)
            command = self._completer.next()
            self._entry.text = " ".join([command] + post_parts)
            if self._completer.single:
                if self._entry.text == command:
                    self._entry.text += " "
                self._caret.position = len(command) + 1
            else:
                self._caret.position = len(command)
        else:
            try: command = layer.command.commands[pre_parts[0].lower().strip()]
            except KeyError:
                print_(_("No command %r found.") % pre_parts[0])
            else:
                print_(command.decl(len(pre_parts) - 2))

    def _on_key_press(self, key, modifiers):
        if key == layer.key.GRAVE:
            self.toggle(not self.visible)
            return pyglet.event.EVENT_HANDLED
        elif key == layer.key.SLASH and not self.visible:
            self.show()
            self.close_on_enter = True
            return pyglet.event.EVENT_HANDLED
        elif self.visible:
            if key == layer.key.RETURN:
                text = self.history.add(self._entry.text)
                if text:
                    res = layer.command.run(text, print_=print_)
                self._entry.text = ""
                if self.close_on_enter:
                    self.hide()
            elif key == layer.key.ESCAPE:
                if self._entry.text:
                    self._entry.text = ""
                else:
                    self.hide()
            elif key == layer.key.TAB:
                self._tab_complete()

            return pyglet.event.EVENT_HANDLED

    def show(self):
        """Show the console."""
        if not self.visible:
            self.close_on_enter = False
            self.visible = True
            self._window.batches.append(self._Batch)
            self._window.push_handlers(self._caret)
            self._window.push_handlers(
                on_text=self._on_text,
                on_text_motion=self._on_text_motion)
            layer.output.hook(self._append_to_log)
            self._caret.visible = True
            self._on_resize(0, 0)

    def hide(self):
        """Hide the console."""
        if self.visible:
            self.close_on_enter = False
            self.visible = False
            self._window.remove_handlers(
                on_text=self._on_text,
                on_text_motion=self._on_text_motion)
            self._window.remove_handlers(self._caret)
            self._caret.visible = False # unschedule the blink
            layer.output.unhook(self._append_to_log)
            self._window.batches.remove(self._Batch)

    def toggle(self, value=None):
        """Set console visibility, returns true if visible."""
        if value is not None:
            if self.visible:
                self.hide()
            else:
                self.show()
        else:
            return self.visible
