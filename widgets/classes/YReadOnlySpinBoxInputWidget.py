
from widgets import YSpinBoxInputWidget

class YReadOnlySpinBoxInputWidget(YSpinBoxInputWidget):

    def __init__(self, parent = None):
        super(YSpinBoxInputWidget, self).__init__(parent)
        try:
            le = filter(lambda x: type(x).__name__ == 'QSpinBox',
            self.children())[0]
            le.setReadOnly(True)
        except IndexError:
            pass
