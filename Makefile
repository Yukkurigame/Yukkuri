
all : widgets editor

widgets : force_look
	$(MAKE) -C widgets/ui

editor : GeneralConfig.py Editor.py

%.py : %.ui
	pyuic4 $< -o $*.py

force_look :
