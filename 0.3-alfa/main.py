#!/usr/bin/env python
import os
import yaml
from xml.etree.ElementTree import XML, parse
import widgets
from widgets import *
from yaml import Loader as _Loader
from yaml import Dumper as _Dumper

_version = '0.3a'

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
                if item.find(".") > 0 and item.rfind("~")+1 < len(item):
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
            return ''
            #raise AttributeError(item)
         

    def __setattr__(self, item, value):
        if self.__dict__.has_key(item):
            dict.__setattr__(self, item, value)
        else:
            self.__setitem__(item, value)

    def save(self):
        stream = file('config', 'w')
        yaml.dump(dict(self), stream)
    
    def load(self):        
        try: stream = file('config', 'rU')
        except: pass
        else:
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
        self.parser = ParseObject()
        self.classes = ClassFactory()
        tabframe = Frame(root)        
        self.mainframe = Frame(root)
        self.fields = {}
        self.loadobj = None
        self.loadfile = None
        self.loaded = None
        self.tabmanager = Tabs(tabframe, self)
        self.pathfrm = self.draw(self.mainframe, pathlbl.getroot())                
        frame = self.draw(self.mainframe, main.getroot())                
        tabframe.pack(side=TOP, fill=X)        
        self.mainframe.pack(side=TOP, expand=YES, fill=BOTH)
        self.pathfrm.pack()
        frame.pack()
        self.dir.change(self.config.path)
        self.changeText(self.fields["path"], str(self.config.path))
        frame.children["ok"].config(command=self.action_ok)
        frame.children["cancel"].config(command=self.root.quit)        

    def draw(self, master, element):
        side="left"        
        if element.tag == "form":           
            if element.attrib.has_key("side"):
                side = element.attrib["side"]
                del element.attrib["side"]
            frame = Frame(master, **element.attrib)            
            for subelement in element:
                widget = self.draw(frame, subelement)
                if widget:                    
                    widget.pack(side=side, fill=X, expand=YES)
            return frame        
        else:
            options = element.attrib
            try: pxml = parse(element.tag+".xml")
            except IOError:            
                if element:
                    options = options.copy()                    
                    for subelement in element:
                        options[subelement.tag] = subelement.text
                if element.tag == "button":
                    if options.has_key("do"):
                        change, type = None, None 
                        do = options["do"]
                        do = eval('self.'+do)
                        try: change = options["change"]
                        except: pass
                        else: del options["change"]
                        try: typeb = eval(options["type"])
                        except: pass                        
                        else:
                            type = []
                            type.append(typeb)                            
                            del options["type"]                        
                        options["command"] = lambda: do(change, type)
                        del options["do"]
                widget_factory = getattr(widgets, element.tag.capitalize())
                if element.tag.capitalize() == "Animation":
                    options["main"] = self
                widget = widget_factory(master, **options)
                if options.has_key("name"):
                    #if self.fields.has_key(options["name"]):
                        #raise KeyError(options["name"])
                    #else:
                    self.fields[options["name"]] = widget 
                return widget
            else:
                frame = Frame(self.tabmanager())                                
                widget = self.draw(frame, pxml.getroot())
                if widget:
                    widget.pack()
                eroot = pxml.getroot()
                options = options.copy()
                #for subelement in eroot:
                    #options[subelement.tag] = subelement.text
                    #widget_factory = getattr(widgets, subelement.tag.capitalize())                
                    #widget_factory(frame, **options).pack()
                tab = self.tabmanager.add_screen(frame, element.tag)
                return tab

    def action_ok(self):
        self.config.save()
        self.saveFile()
        root.quit()

    def saveFile(self, *args):
        if not self.loaded: return
        for field in self.fields:
            if hasattr(self.loadobj, field):
                fld = self.fields[field].get()                
                try: fld = eval(fld)
                except: pass
                setattr(self.loadobj, field, fld)        
        fileObj = open(os.path.join(self.config.path, self.loaded),"w")
        fileObj.write(self.loadobj.dump())
        fileObj.close()         

    def changeText(self, pen, text):
        try: pen.change(text)
        except:            
            pen.delete(0, END)
            pen.insert(0, text)

    def load(self, filename):
        self.loaded = filename
        stream = file(os.path.join(self.config.path, filename), 'rU')
        self.classes.create(self.tabmanager.title.lower().capitalize()+'Def', ParseObject)
        self.loadobj = self.parser.load(stream)
        for field in self.fields:
            if hasattr(self.loadobj, field):
                self.changeText(self.fields[field], getattr(self.loadobj, field))
        self.fields["playAnim"].change()

class _ParseObjectMetaclass(type):

    def __new__(cls, name, bases, kwds):
        kwds.setdefault("parse_tag", "!" + ("." + name).rsplit(".", 1)[1])
        return type.__new__(cls, name, bases, kwds)

    def __init__(cls, name, bases, kwds):
        super(_ParseObjectMetaclass, cls).__init__(name, bases, kwds)
        cls._Loader.add_constructor(cls.parse_tag, cls._from_yaml)
        cls._Dumper.add_representer(cls, cls._to_yaml)

class ParseObject(object):
    __metaclass__ = _ParseObjectMetaclass
    _Loader = _Loader
    _Dumper = _Dumper

    _parse_flow_style = False

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

    def dump(self):
        """Create a string from this object."""
        return yaml.dump(self, Dumper=self._Dumper)

class ClassFactory(object):    
    def create(self, name,  parent, **k):
        cls =  type(name, (parent,), k)
        return cls

class Tabs:
    
    def __init__(self, master, main, side=TOP):
        self.active_fr = None
        self.title = None        
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
        self.config.path = self.main.fields["path"].get()        
        if not self.config.path:            
            self.main.fields["path"].flash()
            #pathfrm = self.main.pathfrm
            #self.main.pathfrm.children["open"].config(activebackground='#0b0')
            #self.main.pathfrm.children["open"].flash()
            #self.main.pathfrm.children["open"].config(activebackground='#fff')
            #self.main.pathfrm.children["plabel"].config(fg='#f00')            
            return
        self.title = title
        if self.active_fr:
            self.active_fr.forget()
        fr.pack(fill=BOTH, expand=1)
        self.active_fr = fr
        self.main.root.title("Make it easy: "+title.capitalize()+" editor "+_version)
        self.change_list(title)
        
    def change_list(self, title):
        self.list.delete(0, END)
        for item in self.main.dir.get_type(title):
            self.list.insert(END, item)

    def onSelect(self, e):
        select=self.list.get(self.list.curselection())
        self.main.load(select)

if __name__ == "__main__":
    root = Tk()
    Main(root)
    root.title("Make it easy "+_version)
    root.mainloop()    

