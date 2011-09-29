
import re

from PyQt4 import QtCore, QtGui

export_classes = ['YSimpleInputWidget', 'YSpinBoxInputWidget']
extern_classes = ['YReadOnlySimpleInputWidget']


def widget_factory(name):
    parent_class = getattr(__import__('widgets.classes.%s' % name,
                globals(), locals(), [name,]), name)
    class _YAbstractWidget(QtGui.QWidget, parent_class):
        def __init__(self, parent = None):
            QtGui.QWidget.__init__(self, parent)
            ui = __import__('ui.Ui_' + name, globals(), fromlist=[name,])
            self.ui = getattr(ui, 'Ui_' + name)()
            self.ui.setupUi(self)

            #parent executes after all things done
            parent_class.__init__(self, parent)

        def setObjectName(self, name):
            super(_YAbstractWidget, self).setObjectName(name)
            try:
                l = filter(lambda x: type(x).__name__ == 'QLabel',
                self.children())[0]
                l.setText(re.sub('_\d+$', '',
                        str(self.objectName())) + ':')
            except IndexError:
                pass

    return _YAbstractWidget

for name in export_classes:
    globals()[name] = widget_factory(name)
for name in extern_classes:
    globals()[name] = getattr(__import__('widgets.classes.%s' % name,
                                fromlist=[name,]), name)

__all__ = export_classes + extern_classes
