import re

class SLPP:

    def __init__(self):
        self.text = ''
        self.ch = ''
        self.at = 0
        self.len = 0
        self.depth = 0
        self.space = re.compile('\s', re.M)
        self.alnum = re.compile('\w', re.M)
        self.newline = '\n'
        self.tab = '\t'

    def decode(self, text):
        if not text or type(text) is not str:
            return
        #FIXME: only short comments removed
        reg = re.compile('--.*$', re.M)
        text = reg.sub('', text, 0)
        self.text = text
        self.at, self.ch, self.depth = 0, '', 0
        self.len = len(text)
        self.next_chr()
        result = self.value()
        return result

    def encode(self, obj):
        if not obj:
            return
        self.depth = 0
        return self.__encode(obj)

    def __encode(self, obj):
        s = ''
        tab = self.tab
        newline = self.newline
        tp = type(obj)
        if tp is str:
            s += '"%s"' % obj
        elif tp in [int, float, long, complex]:
            s += str(obj)
        elif tp is bool:
            s += str(obj).lower()
        elif tp in [list, tuple, dict]:
            self.depth += 1
            if len(obj) == 0 or ( tp is not dict and len(filter( 
                    lambda x:  type(x) in (int,  float,  long) \
                    or (type(x) is str and len(x) < 10),  obj
                )) == len(obj) ):
                newline = tab = ''
            dp = tab * self.depth            
            s += "%s{%s" % (tab * (self.depth - 2), newline)
            if tp is dict:                
                s += (',%s' % newline).join(
                    [self.__encode(v) if type(k) is int \
                        else dp + '%s = %s' % (k, self.__encode(v)) \
                        for k, v in obj.iteritems()
                    ])
            else:
                s += (',%s' % newline).join(
                    [dp + self.__encode(el) for el in obj])
            self.depth -= 1
            s += "%s%s}" % (newline, tab * self.depth)
        return s

    def white(self):
        while self.ch:
            if self.space.match(self.ch):
                self.next_chr()
            else:
                break

    def next_chr(self):
        if self.at >= self.len:
            self.ch = None
            return None
        self.ch = self.text[self.at]
        self.at += 1
        return True

    def value(self):
        self.white()
        if not self.ch:
            return
        if self.ch == '{':
            return self.object()
        if self.ch == "[":
            self.next_chr()
        if self.ch in ['"',  "'",  '[']:
            return self.string(self.ch)
        if self.ch.isdigit() or self.ch == '-':
            return self.number()
        return self.word()

    def string(self,  end=None):
        s = ''
        start = self.ch
        if end == '[':
            end = ']'
        if start in ['"',  "'",  '[']:
            while self.next_chr():
                if self.ch == end:
                    self.next_chr()
                    if start != "[" or self.ch == ']':
                        return s
                s += self.ch
        print "Unexpected end of string while parsing Lua string"

    def object(self):
        o = {}
        k = ''
        idx = 0
        numeric_keys = False
        self.depth += 1
        self.next_chr()
        self.white()
        if self.ch and self.ch == '}':
            self.depth -= 1
            self.next_chr()
            return o #Exit here
        else:
            while self.ch:
                self.white()
                if self.ch == '{':
                    o[idx] = self.object()
                    idx += 1
                    continue
                elif self.ch == '}':
                    self.depth -= 1
                    self.next_chr()
                    if k:
                       o[idx] = k
                    if not numeric_keys and len([ key for key in o if type(key) in (str,  float,  bool,  tuple)]) == 0:
                        ar = []
                        for key in o: 
                           ar.insert(key, o[key])
                        o = ar
                    return o #or here
                else:
                    if self.ch == ',':
                        self.next_chr()
                        continue
                    else:
                        k = self.value()
                        if self.ch == ']':
                            numeric_keys = True
                            self.next_chr()
                    self.white()
                    if self.ch == '=':
                        self.next_chr()
                        self.white()
                        o[k] = self.value()
                        idx += 1
                        k = ''
                    elif self.ch == ',':
                        self.next_chr()
                        self.white()
                        o[idx] = k
                        idx += 1
                        k = ''
        print "Unexpected end of table while parsing Lua string."#Bad exit here

    def word(self):
        s = ''
        if self.ch != '\n':
          s = self.ch
        while self.next_chr():
            if self.alnum.match(self.ch):
                s += self.ch
            else:
                if re.match('^true$', s, re.I):
                    return True
                elif re.match('^false$', s, re.I):
                    return False
                elif s == 'nil':
                    return None
                return str(s)

    def number(self):
        n = ''
        flt = False
        if self.ch == '-':
            n = '-'
            self.next_chr()
            if not self.ch or not self.ch.isdigit():
                print "Malformed number %s(no digits after initial minus)" % self.ch
                return 0
        while self.ch and self.ch.isdigit():
            n += self.ch
            self.next_chr()
        if self.ch and self.ch == '.':
            n += self.ch
            flt = True
            self.next_chr()
            if not self.ch or not self.ch.isdigit():
                print "Malformed number %s (no digits after decimal point)" % self.ch
                return n+'0'
            else:
                n += self.ch
            while self.ch and self.ch.isdigit():
                n += self.ch
                self.next_chr()
        if flt:
            return float(n)
        return int(n)

slpp = SLPP()

__all__ = ['slpp']
