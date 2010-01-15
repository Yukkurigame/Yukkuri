import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory, askopenfilename
from tkColorChooser import askcolor
import os.path

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

class OpenMeta(InputMeta):

    def __init__(self, master, *args, **kwargs):
        super(OpenMeta, self).__init__(master, **kwargs)
        del kwargs["name"]
        for elem in ["label", "entry", "button"]:
            widget_factory = getattr(Tkinter, elem.capitalize())
            kargs = {} #It's bad.
            for k in kwargs.keys():
                if elem is "label" and k != 'text':                
                    continue
                if elem is 'button' and k == 'text':
                     kargs['text'] = "Open"
                     continue
                kargs[k] = kwargs[k]
            widget = widget_factory(self.frame, **kargs)
            self.objects[elem] = widget

    def pack(self, **kwargs):
        super(OpenMeta, self).pack(**kwargs)
        self.objects["button"].config(command=self.Open)

    def Open(self):
        pass

    def flash(self):
        self.objects["button"].config(activebackground='#0b0')
        self.objects["button"].flash()
        self.objects["button"].config(activebackground='#fff')

    def butoncolor(self, color='#ffffff'):
        self.objects["button"].config(activebackground=color)
        self.objects["button"].config(activeforeground=color)
        self.objects["button"].config(background=color)
        self.objects["button"].config(foreground=color)

class Opendir(OpenMeta):
    
    def __init__(self, master, *args, **kwargs):
        super(Opendir, self).__init__(master, **kwargs)

    def Open(self):
        name = askdirectory()
        if name:
            self.change(name)

class Openfile(OpenMeta):

    def __init__(self, master, *args, **kwargs):
        super(Openfile, self).__init__(master, **kwargs)

    def Open(self):
        name = askopenfilename()
        if name:
            name = os.path.basename(name)
            self.change(name)

class Opencolor(OpenMeta):

    def __init__(self, master, *args, **kwargs):
        super(Opencolor, self).__init__(master, **kwargs)
    
    def Open(self):
        color = askcolor(initialcolor=self.get())        
        if not color[0]: return
        self.change(color[1])
        self.butoncolor(color[1])

    def change(self, text=''):
        super(Opencolor, self).change(text)
        self.butoncolor(text)

    def pack(self, **kwargs):
        super(Opencolor, self).pack(**kwargs)
        self.objects["button"].config(relief=SUNKEN)

class Opencolortriple(OpenMeta):

    def __init__(self, master, *args, **kwargs):
        super(Opencolortriple, self).__init__(master, **kwargs)

    def tohex(self, tple):        
        if not tple:
            tple = [255,255,255]
        if type(tple) is not tuple and type(tple) is not list:
            try: tple = eval(tple)
            except: return
        string = '#'
        for i in tple:
            string = string + "%X" % i
        return string

    def Open(self):        
        color = askcolor(initialcolor=self.tohex(self.get()))
        if not color[0]: return
        self.change(str(tuple(color[0])))
        self.butoncolor(color[1])

    def change(self, text=''):        
        super(Opencolortriple, self).change(str(text))
        self.butoncolor(self.tohex(text))

    def pack(self, **kwargs):
        super(Opencolortriple, self).pack(**kwargs)
        self.objects["button"].config(relief=SUNKEN)

class Bool(InputMeta):
    
    def __init__(self, master, *args, **kwargs):
        super(Bool, self).__init__(master, **kwargs)
        label = getattr(Tkinter, "label".capitalize())
        label = label(self.frame, text=kwargs["text"])
        self.objects["label"] = label
        self.var = IntVar()
        for text, value in [("No", 0), ("Yes", 1)]:
            radiobutton = getattr(Tkinter, "radiobutton".capitalize())
            radiobutton = radiobutton(self.frame, text=text, value=value, variable=self.var)
            self.objects["button"+text] = radiobutton

    def change(self, text=''):
        self.var.set(0)        
        text = int(text)
        self.var.set(text)

    def get(self):
        print bool(self.var.get())
        return bool(self.var.get())