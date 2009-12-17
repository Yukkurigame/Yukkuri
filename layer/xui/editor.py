# $Id$

"""layer.editor -- Generic resource viewer / editor
"""

import gtk

import layer.command
import layer.parse

def LoadableKindsComboBox():
    model = gtk.ListStore(object, str)
    model.set_sort_column_id(1, gtk.SORT_ASCENDING)
    for Kind in layer.parse.ParseObject.LoadableKinds:
        model.append(row=[Kind, Kind.__name__])
    cb = gtk.ComboBox(model)
    cell = gtk.CellRendererText()
    cb.pack_start(cell, True)
    cb.add_attribute(cell, 'text', 1)
    return cb

def InstanceList(cb):
    def refresh(cb, tv):
        Kind = cb.get_model()[cb.get_active()][0]
        model = tv.get_model()
        model.clear()
        for instance in Kind.Instances.values():
            model.append(row=[instance, instance.name])

    model = gtk.ListStore(object, str)
    model.set_sort_column_id(1, gtk.SORT_ASCENDING)

    tv = gtk.TreeView(model)
    tv.set_rules_hint(True)
    sw = gtk.ScrolledWindow()
    sw.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
    sw.set_shadow_type(gtk.SHADOW_IN)
    sw.add(tv)
    cell = gtk.CellRendererText()
    column = gtk.TreeViewColumn(_("Instances"), cell)
    column.add_attribute(cell, 'text', 1)
    tv.append_column(column)   
    cb.connect('changed', refresh, tv)

    return sw

def Window():
    w = gtk.Window()
    w.set_title(_("Layer Editor"))
    w.set_default_size(300, 400)
    w.add(gtk.VBox(spacing=6))
    cb = LoadableKindsComboBox()
    w.child.pack_start(cb, fill=True, expand=False)
    tv = InstanceList(cb)
    w.child.pack_start(tv, fill=True, expand=True)
    w.show_all()
    return w

@layer.command.command
def editor():
    """Initialize the in-game editor."""

    Window().present()

