#!/usr/bin/env python
import os
import yaml
from xml.etree.ElementTree import XML, parse
import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory
from yaml import Loader as _Loader

class CurrentDir(list):
    
    def get_type(self, type):
        ret = []
        for item in list(self):
            if item.find('.'+str(type)) > 0:
                ret.append(item)        
        return ret
    
    def change(self, path):
        if not path: return
        self.clear()
        for item in os.listdir(path):
            if os.path.isfile(os.path.join(path, item)):
                self.append(item)

    def clear(self):
        for item in list(self):
            index = self.index(item)
            self.pop(index)

class Config(dict):

    def __init__(self, indict=None):
        if indict is None:
            indict = {}        
        dict.__init__(self, indict)
        self.load()

    def __getattr__(self, item):
        try:
            return self.__getitem__(item)
        except KeyError:
            raise AttributeError(item)

    def __setattr__(self, item, value):
        if self.__dict__.has_key(item):
            dict.__setattr__(self, item, value)
        else:
            self.__setitem__(item, value)

    def save(self):
        stream = file('config', 'w')
        yaml.dump(dict(self), stream)
    
    def load(self):
        stream = file('config', 'rU')
        d = yaml.load(stream)
        for key in d.keys():
            setattr(self, key, d[key])

class Main:

    def __init__(self, root):
        main = parse("main.xml")
        pathlbl = parse("path.xml")
        self.root = root
        self.config = Config()        
        self.dir = CurrentDir()
        self.parser = Parser()
        tabframe = Frame(root)        
        self.mainframe = Frame(root)
        self.tabmanager = Tabs(tabframe, self)
        self.pathfrm = self.draw(self.mainframe, pathlbl.getroot())                
        frame = self.draw(self.mainframe, main.getroot())                
        tabframe.pack(side=TOP, fill=X)        
        self.mainframe.pack(side=TOP, expand=YES, fill=BOTH)
        self.pathfrm.pack()        
        frame.pack()
        self.changeDir()
        self.pathfrm.children["open"].config(command=self.openDir)
        frame.children["ok"].config(command=self.action_ok)
        frame.children["cancel"].config(command=self.root.quit)

    def draw(self, master, element):                
        if element.tag == "form":
            frame = Frame(master, **element.attrib)
            for subelement in element:                
                widget = self.draw(frame, subelement)
                if widget:                    
                    widget.pack(side="left")
            return frame        
        else:
            options = element.attrib
            try: pxml = parse(element.tag+".xml")
            except IOError:            
                if element:
                    options = options.copy()
                    for subelement in element:
                        options[subelement.tag] = subelement.text
                widget_factory = getattr(Tkinter, element.tag.capitalize())                            
                return widget_factory(master, **options)
            else:
                frame = Frame(self.tabmanager())
                eroot = pxml.getroot()
                options = options.copy()
                for subelement in eroot:
                    options[subelement.tag] = subelement.text
                    widget_factory = getattr(Tkinter, subelement.tag.capitalize())                
                    widget_factory(frame, **options).pack()
                tab = self.tabmanager.add_screen(frame, element.tag)
                return tab

    def action_ok(self):
        self.config.save()
        root.quit()

    def openDir(self):
        self.config.path = askdirectory()
        self.pathfrm.children["plabel"].config(fg='#000')
        self.changeDir()
        #self.change_list()
   
    def changeDir(self):
        pen = self.pathfrm.children["path"]        
        self.dir.change(self.config.path)
        pen.delete(0, END)
        pen.insert(0, str(self.config.path))

    def load(self, filename):
        stream = file(os.path.join(self.config.path, filename), 'rU')
        
        print yaml.load(stream)

class Parser:

    @classmethod
    def _from_yaml(cls, loader, node):
        return loader.construct_yaml_object(node, cls)

    @classmethod
    def _to_yaml(cls, dumper, data):
        return dumper.represent_yaml_object(
            cls.parse_tag, data, cls, flow_style=cls._parse_flow_style)

    @classmethod
    def load(cls, data):
        """Create an object from data, fail if it's the wrong type."""
        result = yaml.load(data, Loader=cls._Loader)
        if not isinstance(result, cls):
            raise TypeError(
                "Loaded data type %r != loader type %r" % (type(data), cls))
        return result

    @classmethod
    def load_list(cls, data):
        """Create a list of objects from data, fail if it's the wrong type.
        If data describes a single object, a list containing it is
        returned.
        """
        result = load(data)
        if isinstance(result, list) and result:
            types = map(type, result)
            if not (max(types) == min(types) == cls):
                raise TypeError(
                    "Loaded list types %r, %r != loader type %r" %
                    (max(types), min(types), cls))
        elif not isinstance(result, cls):
            raise TypeError(
                "Loaded data type %r != loader type %r" % (type(data), cls))
        elif not isinstance(result, list):
            result = [result]
        return result

    def dump(self):
        """Create a string from this object."""
        return yaml.dump(self, Dumper=self._Dumper)

    def constructor(self, loader, node):
        value = loader.construct_scalar(node)
        print value
    
    def add(self, type):        
        yaml.add_constructor(u'!'+type.lower().capitalize()+'Def', self._from_yaml)

class Tabs:
    
    def __init__(self, master, main, side=TOP):
        self.active_fr = None        
        self.count = 0
        self.main = main
        self.config = main.config
        self.choice = -1
        self.side = side        
        self.rb_fr = Frame(master, borderwidth=2, relief=RIDGE)
        self.rb_fr.pack(side=side, fill=BOTH)
        self.list_fr = Frame(master, borderwidth=2, relief=RIDGE)
        self.list_fr.pack(side=side, fill=BOTH)
        sbar = Scrollbar(self.list_fr)
        self.list = Listbox(self.list_fr, relief=SUNKEN, height=3)
        sbar.config(command=self.list.yview)                
        self.list.config(yscrollcommand=sbar.set)        
        sbar.pack(side=RIGHT, fill=Y)        
        self.list.pack(side=RIGHT, expand=YES, fill=BOTH)
        self.list.bind('<Double-1>', self.onSelect)
        self.screen_fr = Frame(main.mainframe, borderwidth=2, relief=RIDGE)
        self.screen_fr.pack(fill=BOTH)
    
    def __call__(self):
        return self.screen_fr

    def add_screen(self, fr, title):
        b = Radiobutton(self.rb_fr, text=title, indicatoron=0, \
                variable=self.choice, value=self.count, \
                command=lambda: self.display(fr, title))
        b.pack(fill=BOTH, side=self.side)
        self.count += 1
        return b

    def display(self, fr, title):
        if not self.config.path:
            pathfrm = self.main.pathfrm
            self.main.pathfrm.children["open"].config(activebackground='#0b0')
            self.main.pathfrm.children["open"].flash()
            self.main.pathfrm.children["open"].config(activebackground='#fff')
            self.main.pathfrm.children["plabel"].config(fg='#f00')            
            return
        if self.active_fr:
            self.active_fr.forget()
        fr.pack(fill=BOTH, expand=1)
        self.active_fr = fr
        self.main.root.title("Make it easy: "+title.capitalize()+" editor")
        self.change_list(title)
        
    def change_list(self, title):
        self.list.delete(0, END)
        for item in self.main.dir.get_type(title):
            self.list.insert(END, item)
        self.main.parser.add(title)

    def onSelect(self, e):
        print e
        select=self.list.get(self.list.curselection())
        self.main.load(select)

if __name__ == "__main__":
    root = Tk()
    Main(root)
    root.title("Make it easy")
    root.mainloop()    

