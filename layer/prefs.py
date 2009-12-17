# $Id$

import sys
from layer._prefs import Preferences
Preferences.Preferences = Preferences
prefs = Preferences()
sys.modules[__name__] = prefs
