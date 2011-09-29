
WGT = Ui_YSimpleInputWidget.ui Ui_YSpinBoxInputWidget.ui

SCRIPTS = $(WGT:.ui=.py)

all : $(SCRIPTS)

%.py : %.ui
	pyuic4 $< -o $*.py
