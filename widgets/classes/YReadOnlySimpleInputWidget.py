
from widgets import YSimpleInputWidget

class YReadOnlySimpleInputWidget(YSimpleInputWidget):

    def __init__(self, parent = None):
        super(YReadOnlySimpleInputWidget, self).__init__(parent)
        try:
            le = filter(lambda x: type(x).__name__ == 'QLineEdit',
            self.children())[0]
            le.setReadOnly(True)
        except IndexError:
            pass


