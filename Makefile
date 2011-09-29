
all : widgets editor

widgets : force_look
	$(MAKE) -C widgets/ui

editor : Editor.py

Editor.py : Editor.ui
	pyuic4 Editor.ui -o Editor.py

force_look :
	