#!/usr/bin/env python
import os
from xml.etree.ElementTree import XML, parse
import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory

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

class Main:

    def __init__(self, root):
        main = parse("main.xml")
        pathlbl = parse("path.xml")
        self.path = None
        self.root = root
        self.dir = CurrentDir()
        tabframe = Frame(root)        
        self.mainframe = Frame(root)
        self.tabmanager = Tabs(tabframe, self)
        self.pathfrm = self.draw(self.mainframe, pathlbl.getroot())                
        frame = self.draw(self.mainframe, main.getroot())                
        tabframe.pack(side=TOP, fill=X)        
        self.mainframe.pack(side=TOP, expand=YES, fill=BOTH)
        self.pathfrm.pack()        
        frame.pack()
        self.pathfrm.children["open"].config(command=self.opendir)
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
       root.quit()

    def opendir(self):
        self.path = askdirectory()
        self.pathfrm.children["plabel"].config(fg='#000')
        pen = self.pathfrm.children["path"]        
        self.dir.change(self.path)
        pen.delete(0, END)
        pen.insert(0, str(self.path))
        #self.change_list()
        

class Tabs:
    
    def __init__(self, master, main, side=TOP):
        self.active_fr = None        
        self.count = 0
        self.main = main
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
        if not self.main.path:
            pathfrm = self.main.pathfrm
            self.main.pathfrm.children["open"].config(activebackground='#0b0')
            self.main.pathfrm.children["open"].flash()
            self.main.pathfrm.children["open"].config(activebackground='#fff')
            self.main.pathfrm.children["plabel"].config(fg='#f00')
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


if __name__ == "__main__":
    root = Tk()
    Main(root)
    root.title("Make it easy")
    root.mainloop()    

