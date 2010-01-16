import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory, askopenfilename
from tkColorChooser import askcolor
import os.path
import Image, ImageTk

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

class Animation(InputMeta):
    
    def __init__(self, master, *args, **kwargs):
        self.width, self.height = 10, 10        
        self.activeimages = None
        self.activeimage = None
        self.main = kwargs["main"]
        del kwargs["main"]
        super(Animation, self).__init__(master, **kwargs)
        self.imageframe = Frame(self.frame, padx=5, pady=5)
        self.uiframe = Frame(self.frame)
        canvas = getattr(Tkinter, "canvas".capitalize())        
        self.canvas = canvas(self.imageframe, width=self.width, height=self.height, relief=GROOVE, bd=2)
        self.objects["canvas"] = self.canvas
        self.var = StringVar()
        fields = self.get_opts()        
        opts = getattr(Tkinter, "OptionMenu")
        self.opts = opts(self.uiframe, self.var, *fields, command=self.select)
        button = getattr(Tkinter, "button".capitalize())        
        self.button = button(self.uiframe, text="Next", command=self.next_image)

    def change(self):
        self.width = int(self.main.fields["width"].get())
        self.height = int(self.main.fields["height"].get())
        self.objects["canvas"].config(width=int(self.width+4), height=int(self.height+4))
        self.mainimage = None

    def pack(self, *args, **kwargs):
        self.button.pack(side=TOP,expand=YES, fill=X)
        self.opts.pack(side=TOP,expand=YES, fill=X)
        self.imageframe.pack(side=LEFT)
        self.uiframe.pack(side=LEFT,expand=YES, fill=X)
        super(Animation, self).pack(*args, **kwargs)

    def get_opts(self):
        fields = self.frame.master.children.keys()
        fields.sort()
        fields.remove(self.frame._name)
        fields.remove("animation_label")
        return fields

    def select(self, select):
        try: field = eval(self.main.fields[select].get())
        except: return
        self.activeimages = self.get_images(*field)
        self.activeimage = None
        self.next_image()
        
    def load_image(self):         
        try: image = self.main.fields["name"].get().lower()+'.png'
        except: return        
        path =  os.path.join(self.main.config.path, '..', 'images' , image)
        try: image = Image.open(path)
        except:
            print "Failed to load "+path 
            return
        self.mainimage = image
        size = image.size
        cols = int(size[0] / int(self.width))
        rows =  int(size[1] / int(self.height))
        bboxes = []
        width = int(self.width)
        height = int(self.height)
        for row in range(rows):
            mbox = []
            for col in range(cols):
                box = (col*width,
                           row*height+1,
                           int(width+col*width-1),
                           int(height+row*height))
                bbox = image.crop(box)
                tkimage = ImageTk.PhotoImage(bbox)
                mbox.append(tkimage)
            mbox.reverse()
            for el in mbox:
                bboxes.append(el)
        bboxes.reverse() #artful plan
        self.bboxes = bboxes

    def get_images(self, start, offset=None):
        if not offset: return
        if not self.mainimage: self.load_image()
        return self.bboxes[start:start+offset+1]

    def next_image(self):
        if not self.activeimages: return
        if not self.activeimage or self.activeimage >= len(self.activeimages): 
            self.activeimage = 0
        self.objects["canvas"].delete(ALL)
        self.objects["canvas"].create_image(4, 4, anchor=NW, image=self.activeimages[self.activeimage])
        self.activeimage += 1