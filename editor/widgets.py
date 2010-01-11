import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory, askopenfilename

class InputMeta(object):

    def __init__(self, master, **kwargs):
        self.objects = {}        
        self.frame = Frame(master, name=kwargs["name"])
        del kwargs["name"]

    def pack(self, *args, **kwargs):
        keys = self.objects.keys()
        keys.sort(reverse=True)
        for elem in keys:
            if elem == 'label': expand = 'no'
            else: expand = 'yes'
            self.objects[elem].pack(side=LEFT,expand=expand, fill=X)
        self.frame.pack(**kwargs)

    def get(self):
        get = self.objects['entry'].get()
        if get != '':
            return get

    def change(self, text=''):
        print self.objects['entry']
        self.objects['entry'].delete(0, END)
        self.objects['entry'].insert(0, text)

    def delete(self, start, end):
        self.objects['entry'].delete(start, end)

    def insert(self, start, text):
        self.objects['entry'].insert(start, text)

class Input(InputMeta):
    
    def __init__(self, master, *args, **kwargs):
        super(Input, self).__init__(master, **kwargs)
        for elem in ["label", "entry"]:
            widget_factory = getattr(Tkinter, elem.capitalize())
            kargs = {} #It's bad.             
            for k in kwargs.keys():                
                if elem is "label" and k != 'text':                     
                    continue
                kargs[k] = kwargs[k]            
            widget = widget_factory(self.frame, **kargs)
            self.objects[elem] = widget

class Opendir(InputMeta):
    
    def __init__(self, master, *args, **kwargs):
        super(Opendir, self).__init__(master, **kwargs)
        del kwargs["name"]
        for elem in ["label", "entry", "button"]:
            widget_factory = getattr(Tkinter, elem.capitalize())
            kargs = {} #It's bad.
            for k in kwargs.keys():
                if elem is "label" and k != 'text':                
                    continue
                #if k == 'name': continue
                if elem is 'button' and k == 'text':
                     kargs['text'] = "Open"
                     continue
                kargs[k] = kwargs[k]            
            widget = widget_factory(self.frame, **kargs)
            self.objects[elem] = widget

    def pack(self, **kwargs):
        super(Opendir, self).pack(**kwargs)
        self.objects["button"].config(command=self.OpenDir) #lambda: self.OpenDir(self.objects["entry"]))

    def OpenDir(self):
        return askdirectory() #initialdir=self.config.path

    def flash(self):
        self.objects["button"].config(activebackground='#0b0')
        self.objects["button"].flash()
        self.objects["button"].config(activebackground='#fff')
                
