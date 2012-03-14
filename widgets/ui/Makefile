
WGT = Ui_YSimpleInputWidget.ui Ui_YSpinBoxInputWidget.ui \
      Ui_YDoubleSpinBoxInputWidget.ui Ui_YColorInputWidget.ui\
      Ui_YFileInputWidget.ui Ui_YComboBoxWidget.ui Ui_YImageChooser.ui \
      Ui_YImageViewWidget.ui Ui_YTableWidget.ui Ui_YTreeDialog.ui \
      Ui_YAnimationPreviewWidget.ui Ui_YScrollBarInputWidget.ui

SCRIPTS = $(WGT:.ui=.py)

all : $(SCRIPTS)

%.py : %.ui
	pyuic4 $< -o $*.py
