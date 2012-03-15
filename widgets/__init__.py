
import re

from PyQt4 import QtCore, QtGui

export_classes = ['YSimpleInputWidget', 'YSpinBoxInputWidget',
                'YDoubleSpinBoxInputWidget', 'YColorInputWidget',
                'YFileInputWidget', 'YComboBoxWidget', 'YImageChooser',
                'YImageViewWidget', 'YTableWidget', 'YAnimationPreviewWidget',
                'YScrollBarInputWidget']
extern_classes = ['YReadOnlySimpleInputWidget', 'YReadOnlySpinBoxInputWidget']


def widget_factory(name):
    try:
        parent_class = getattr(__import__('widgets.classes.%s' % name,
                    globals(), locals(), [name,]), name)
    except ImportError, e:
        print str(e)
        parent_class = QtCore.QObject
    if not issubclass(parent_class, QtGui.QWidget):
        main_class = QtGui.QWidget
    else:
        main_class = QtCore.QObject
    if issubclass(parent_class, main_class):
        first_class, second_class = parent_class, main_class
    else:
        first_class, second_class = main_class, parent_class
    class _YAbstractWidget(first_class, second_class):
        def __init__(self, parent = None):
            main_class.__init__(self, parent)
            ui = __import__('ui.Ui_' + name, globals(), fromlist=[name,])
            self.ui = getattr(ui, 'Ui_' + name)()
            self.ui.setupUi(self)
            #parent executes after all things done
            parent_class.__init__(self, parent)

        def setObjectName(self, name):
            super(_YAbstractWidget, self).setObjectName(name)
            try:
                l = filter(lambda x: type(x) == QtGui.QLabel,
                self.children())[0]
                t = re.sub('_\d+$', '', str(self.objectName()))
                t = re.sub('_', ' ', t)
                l.setText(t + ':')
            except IndexError:
                pass

        def getValue(self):
            try:
                return super(_YAbstractWidget, self).getValue()
            except AttributeError:
                pass
            ret = []
            lines = filter(lambda x: type(x) in [QtGui.QLineEdit,
                        QtGui.QSpinBox, QtGui.QDoubleSpinBox,
                        QtGui.QCheckBox, QtGui.QComboBox],
                        self.children())
            for line in lines:
                ltype = type(line)
                t = ''
                if ltype == QtGui.QLineEdit:
                    t = str(line.text())
                    if re.match("^\d+\.\d+$", t):
                        t = float(t)
                    elif re.match("^\d+$", t):
                        t = int(t)
                elif ltype == QtGui.QSpinBox:
                    t = line.value()
                elif ltype == QtGui.QDoubleSpinBox:
                    t = line.value()
                elif ltype == QtGui.QCheckBox:
                    t = bool(line.isChecked())
                elif ltype == QtGui.QComboBox:
                    t = str(line.currentText())
                ret.append(t)
            if not len(ret):
                return
            if len(ret) < 2:
                ret = ret[0]
            return ret

        def setValue(self, *data):
            lines = filter(lambda x: type(x) in [QtGui.QLineEdit,
                            QtGui.QSpinBox, QtGui.QDoubleSpinBox,
                            QtGui.QCheckBox, QtGui.QComboBox],
                            self.children())
            if len(lines):
                values = []
                if type(data) != list and type(data) != tuple:
                    data = [data,]
                for i in range(0, len(lines)):
                    ltype = type(lines[i])
                    field = ''
                    if i < len(data):
                        field = data[i]
                    if ltype == QtGui.QLineEdit:
                        field = str(field)
                        lines[i].setText(field)
                    elif ltype == QtGui.QSpinBox:
                        if not field:
                            field = 0
                        try:
                            field = int(field)
                            lines[i].setValue(field)
                        except:
                            pass
                    elif ltype == QtGui.QDoubleSpinBox:
                        if not field:
                            field = 0.0
                        field = float(field)
                        lines[i].setValue(field)
                    elif ltype == QtGui.QCheckBox:
                        field = bool(field)
                        if not field:
                            field = False
                        lines[i].setChecked(field)
                    elif ltype == QtGui.QComboBox:
                        field = lines[i].findText(str(field))
                        if field >= 0:
                            lines[i].setCurrentIndex(field)
                    values.append(field if type(field) is not str else QtCore.QString(field))
                self._changed(*values)

        def _changed(self, *values):
            types = ', '.join(map(lambda x: type(x).__name__, values))
            self.emit(QtCore.SIGNAL("valueChanged(%s)" % types), *values)

    return _YAbstractWidget

for name in export_classes:
    globals()[name] = widget_factory(name)
for name in extern_classes:
    globals()[name] = getattr(__import__('widgets.classes.%s' % name,
                                fromlist=[name,]), name)

__all__ = export_classes + extern_classes
