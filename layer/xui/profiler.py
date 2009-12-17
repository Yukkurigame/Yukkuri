import os

from cProfile import Profile

import gobject
import gtk
import pango

__all__ = ["pyglet", "GProfiler"]

UI = """<ui>
  <menubar name='Menu'>
    <menu action='Profiles'>
      <menuitem action="OpenProfile" />
      <menuitem action="SaveProfile" />
      <menuitem action="ExportProfile" />
      <separator/>
      <menuitem action="Quit" />
    </menu>
    <menu action='Help'>
      <menuitem action='About'/>
    </menu>
  </menubar>
</ui>
"""

# If the user hasn't set up gettext translation, fake it.
try: _
except NameError: _ = lambda s: s

class ProfileModel(gtk.ListStore):
    @staticmethod
    def matches(model, column, key, iter):
        filename = model[iter][1]
        callname = model[iter][2]
        if key in filename.lower():
            return False
        elif key in callname.lower():
            return False
        else:
            return True

    def __init__(self):
        super(ProfileModel, self).__init__(
            object, # entry
            str, # filename
            str, # callable name
            int, # line number
            int, # call count
            int, # recursive call count
            float, # total time
            float, # inline time
            )
        self.stats = []
        self._nonmodules = set()

    def _get_stats(self):
        return [row[0] for row in self]

    def _set_stats(self, stats):
        self.clear()
        self._stats = stats
        self._nonmodules = set()
        filenames = set()
        for entry in stats:
            if isinstance(entry.code, str):
                filename = ""
                callname = entry.code
                line = 0
            else:
                filename = entry.code.co_filename
                filenames.add(filename)
                callname = entry.code.co_name
                line = entry.code.co_firstlineno
            self.append(row=[entry,
                             filename,
                             callname,
                             line,
                             entry.callcount,
                             entry.reccallcount,
                             entry.totaltime,
                             entry.inlinetime])

    stats = property(_get_stats, _set_stats)

class FilterEntry(gtk.Entry):
    def __init__(self, model):
        super(FilterEntry, self).__init__()
        self.connect_object('changed', self._compile, model)
        self.set_tooltip_text(
            _("Enter search terms. Use '!' as a prefix to exclude terms."))
        model.set_visible_func(self.filter)
        self._include = []
        self._exclude = []

    def _compile(self, model):
        self._include = []
        self._exclude = []
        for token in self.get_text().lower().split():
            if token[0] == '!':
                self._exclude.append(token[1:])
            else:
                self._include.append(token)
        model.refilter()
        
    def filter(self, model, iter):
        try: code = model[iter][0].code
        except AttributeError:
            return False
        if isinstance(code, str): text = code
        else: text = code.co_name + " " + code.co_filename
        text = text.lower()
        for token in self._exclude:
            if token in text:
                return False
        for token in self._include:
            if token not in text:
                return False
        return True

class GProfiler(gtk.Window):
    """An interactive graphical profiler.

    It is not visible by default; you will need to call .show() on
    the returned window.
    """
    def __init__(self):
        super(GProfiler, self).__init__()
        self.set_title(_("Layer Profiler"))
        self.set_default_size(400, 300)
        self.profile = Profile()
        self.add(gtk.VBox())

        self.ui = gtk.UIManager()
        actions = gtk.ActionGroup("ProfilerActions")
        actions.add_actions(
            [('Profiles', None, _("_Profiles")),
             ('OpenProfile', gtk.STOCK_OPEN, None,
              "<control>O", None, None),
             ('SaveProfile', gtk.STOCK_SAVE, None,
              "<control>S", None, None),
             ('ExportProfile', gtk.STOCK_CONVERT, _("_Export as Text"),
              "<control>E", None, None),
             ("Quit", gtk.STOCK_QUIT, None, None, None,
              lambda a: self.destroy()),
             ("Help", None, _("_Help")),
             ("About", gtk.STOCK_ABOUT, None, None, None, None),
             ])

        self.ui.insert_action_group(actions, -1)
        self.ui.add_ui_from_string(UI)
        self.add_accel_group(self.ui.get_accel_group())
        self.child.pack_start(self.ui.get_widget("/Menu"), expand=False)

        toolbar = gtk.Toolbar()
        self.child.pack_start(toolbar, expand=False, fill=True)
        self.model = ProfileModel()
        filtered = self.model.filter_new()
        align = gtk.Alignment(xscale=1.0, yscale=1.0)
        align.set_padding(0, 0, 6, 0)
        label = gtk.Label()
        self.entry = FilterEntry(filtered)
        label.set_mnemonic_widget(self.entry)
        label.set_text_with_mnemonic(_("_Filter:"))
        self.view = gtk.TreeView(gtk.TreeModelSort(filtered))
        self.view.connect('row-activated', self._open_file)
        self.view.set_enable_search(True)
        self.view.set_search_equal_func(self.model.matches)
        sw = gtk.ScrolledWindow()
        sw.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        sw.set_shadow_type(gtk.SHADOW_IN)
        sw.add(self.view)
        box = gtk.HBox(spacing=3)
        box.pack_start(label, expand=False)
        box.pack_start(self.entry)
        align.add(box)
        self.child.pack_start(align, expand=False)
        self.child.pack_start(sw, expand=True)

        cell = gtk.CellRendererText()
        cell.props.ellipsize = pango.ELLIPSIZE_MIDDLE

        column = gtk.TreeViewColumn(_("Filename"), cell)
        column.add_attribute(cell, 'text', 1)
        column.set_sort_column_id(2)
        column.set_resizable(True)
        column.set_expand(True)
        self.view.append_column(column)   

        column = gtk.TreeViewColumn(_("Function"), cell)
        column.add_attribute(cell, 'text', 2)
        column.set_sort_column_id(2)
        column.set_resizable(True)
        column.set_expand(True)
        self.view.append_column(column)   

        cell = gtk.CellRendererText()
        column = gtk.TreeViewColumn(_("Call #"), cell)
        column.add_attribute(cell, 'text', 4)
        column.set_sort_column_id(4)
        self.view.append_column(column)   
        column = gtk.TreeViewColumn(_("Total"), cell)
        column.add_attribute(cell, 'text', 6)
        column.set_sort_column_id(6)
        self.view.append_column(column)   
        column = gtk.TreeViewColumn(_("Inline"), cell)
        column.add_attribute(cell, 'text', 7)
        column.set_sort_column_id(7)
        self.view.append_column(column)   

        self.totalstats = gtk.Statusbar()
        self.child.pack_start(self.totalstats, expand=False)
        
        toggle = gtk.ToggleToolButton(gtk.STOCK_MEDIA_RECORD)
        toggle.set_active(False)
        toggle.connect('toggled', self.toggle)
        toolbar.insert(toggle, 0)
        self.entry.grab_focus()
        self.running = False
        self.child.show_all()

    def _open_file(self, view, path, column):
        code = view.get_model().model[path][0].code
        try:
            lineno = "+%d" % code.co_firstlineno
            filename = code.co_filename
        except AttributeError: pass
        else:
            args = ['sensible-editor', lineno, filename]
            gobject.spawn_async(args, flags=gobject.SPAWN_SEARCH_PATH)

    def toggle(self, button):
        """Turn profiling on if off and vice versa."""
        if button.get_active():
            self.start()
        else:
            self.stop()

    def snapshot(self):
        """Update the UI with a snapshot of the current profile state."""
        self.model.stats = self.profile.getstats()
        totalcalls = 0
        totaltime = 0
        for entry in self.model.stats:
            totalcalls += entry.callcount
            totaltime += entry.inlinetime
        text = _("%(calls)d calls in %(time)f CPU seconds.") % dict(
            calls=totalcalls, time=totaltime)
        self.totalstats.pop(0)
        self.totalstats.push(0, text)
        

    def start(self):
        """Start profiling (adding to existing stats)."""
        if not self.running:
            self.profile.enable()
            self.running = True

    def stop(self):
        """Stop profiling (but retain stats)."""
        if self.running:
            self.profile.disable()
            self.running = False
            self.snapshot()

def pyglet():
    """Hook GTK+ into the pyglet main loop.

    After calling this you can run GProfiler() as necessary, and it
    will work correctly within a running pyglet application.
    """
    import pyglet
    def _gtk(dt):
        while gtk.events_pending():
            gtk.main_iteration()
    pyglet.clock.schedule(_gtk)
            
if __name__ == "__main__":
    w = GProfiler()
    w.connect('destroy', gtk.main_quit)
    w.show()
    gtk.main()
