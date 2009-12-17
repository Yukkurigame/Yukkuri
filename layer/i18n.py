# $Id: i18n.py 262 2009-03-16 06:20:38Z piman $

import __builtin__
import gettext
import locale

class GlibTranslations(gettext.GNUTranslations):
    """Provide a glib-like translation API for Python.

    This class adds support for qgettext (and uqgettext) mirroring
    glib's Q_ macro, which allows for disambiguation of identical
    source strings. It also installs N_, Q_, and ngettext into the
    __builtin__ namespace.
    """

    plural = lambda n: n != 1

    def __init__(self, *args, **kwargs):
        self._catalog = {}
        gettext.GNUTranslations.__init__(self, *args, **kwargs)

    def qgettext(self, msgid):
        msgstr = self.gettext(msgid)
        if msgstr == msgid:
            try: return msgstr.split("|", 1)[1]
            except IndexError: return msgstr
        else:
            return msgstr

    def uqgettext(self, msgid):
        msgstr = self.ugettext(msgid)
        if msgstr == msgid:
            try: return msgstr.split(u"|", 1)[1]
            except IndexError: return msgstr
        else:
            return msgstr

    def install(self, unicode=True):
        if unicode:
            _ = self.ugettext
            _Q = self.uqgettext
            ngettext = self.ungettext
            _N = unicode
        else:
            _ = self.gettext
            _Q = self.qgettext
            ngettext = self.ngettext
            _N = lambda s: s

        __builtin__.__dict__["_"] = _
        __builtin__.__dict__["Q_"] = _Q
        __builtin__.__dict__["N_"] = _N
        __builtin__.__dict__["ngettext"] = ngettext

def install(program=None):
    try:
        locale.setlocale(locale.LC_ALL, '')
    except locale.Error:
        pass

    t = None
    if program:
        try:
            t = gettext.translation(program, class_=GlibTranslations)
        except IOError:
            pass
    if t is None:
        t = GlibTranslations()
    t.install(unicode=True)

__all__ = ["install"]
