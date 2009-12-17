# Copyright (c) 1998-2001 by Secret Labs AB.  All rights reserved.
# Copyright 2008 Joe Wreschnig
#
# This code can be redistributed under CNRI's Python 1.6 license.  For
# any other use, please contact Secret Labs AB (info@pythonware.com).
#
# Based on re.Scanner, copied because that one likes moving around.
#
# $Id: lexer.py 333 2009-04-07 08:29:27Z joe.wreschnig $

#!!! layer.lexer cannot depend on any other layer modules. !!!#

# TODO: Rewrite this to not use private sre modules.

import sre_compile
import sre_parse

class Lexer(object):
    def __init__(self, lexicon, flags=0):
        self.__lexicon = lexicon
        # combine phrases into a compound pattern
        p = []
        s = sre_parse.Pattern()
        s.flags = flags
        for phrase, action in lexicon:
            p.append(sre_parse.SubPattern(s, [
                (sre_parse.SUBPATTERN,
                 (len(p) + 1, sre_parse.parse(phrase, flags))),
                ]))
        s.groups = len(p) + 1
        p = sre_parse.SubPattern(s, [(sre_parse.BRANCH, (None, p))])
        self.__scanner = sre_compile.compile(p)

    def scan(self, string, replace={}):
        result = []
        append = result.append
        match = self.__scanner.scanner(string).match
        i = 0
        while True:
            m = match()
            if not m:
                break
            j = m.end()
            if i == j:
               break
            action = self.__lexicon[m.lastindex - 1][1]
            if action is not None:
                append(action(m.group(), replace))
            i = j
        return result, string[i:]
