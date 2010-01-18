import Tkinter
from Tkinter import *
from tkFileDialog import askdirectory, askopenfilename, asksaveasfilename
from tkColorChooser import askcolor
import os
import ImageTk, Image
from math import ceil

class InputMeta(object):

    def __init__(self, master, **kwargs):
        self.objects = {}        
        self.frame = Frame(master, name=kwargs["name"])
        del kwargs["name"]

    def pack(self, *args, **kwargs):
        try: keys = self.objects.keys()
        except: keys =[]
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

    def bind(self, *args, **kwargs):
        self.objects["entry"].bind(*args, **kwargs)

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
        self.frame = self.main.load_xml(master, "playanim")
        self.uiframe = self.frame.children["anim_frameleft"].children["anim_uiframe"]
        self.canvas =  self.main.fields["anim_img_canvas"]
        self.var = StringVar()
        fields = self.get_opts()
        self.opts = OptionMenu(self.uiframe, self.var, *fields, command=self.select)
        self.main.fields["next_image_frame"].configure(command=self.next_image)
        self.main.fields["open_image_creator"].configure(command=self.open_creator)
        self.imagemaker = ImageMaker(self.main)
        self.imagemaker.pack()

    def change(self):
        self.width = int(self.main.fields["width"].get())
        self.height = int(self.main.fields["height"].get())
        self.canvas.config(width=int(self.width+4), height=int(self.height+4))
        self.mainimage = None

    def pack(self, *args, **kwargs):
        self.opts.pack(side=TOP,expand=YES, fill=X)
        self.uiframe.pack(expand=YES)
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
        image = self.main.load_image(os.path.join('..', 'images' , image))
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
        self.canvas.delete(ALL)
        self.canvas.create_image(4, 4, anchor=NW, image=self.activeimages[self.activeimage])
        self.activeimage += 1

    def open_creator(self):
        self.imagemaker.show()

class ImageMaker(InputMeta):

    def __init__(self, main, **kwargs):
        self.main = main
        self.window = self.__create()
        self.images = []
        #super(ImageMaker, self).__init__(self.window, name="imagemaker")                
        self.main.fields["imagemaker"] = self
        self.main.fields["creator_close"].configure(command=self.hide)
        self.dpioffset = ceil(self.canvas.winfo_fpixels('1i'))/72.0
        self.advoffset = 5*self.dpioffset

    def __create(self):
        imagemaker = Toplevel(self.main.root)
        imagemaker.title('Make it easy: ImageMaker "I have not mastered photoshop"')
        imagemaker.protocol("WM_DELETE_WINDOW", self.hide)
        self.imagemaker = imagemaker
        self.__fill(imagemaker)
        imagemaker.withdraw()
        return imagemaker

    def hide(self):
        self.window.withdraw()

    def show(self):
        self.window.state("normal")
        image = self.main.load_image('cirno.jpg', '.')
        self.canvas_draw(*image.size, img=image)

    def canvas_draw(self, x, y, img=None):        
        self.canvas.configure(width=x-self.advoffset, height=y-self.advoffset)        
        self.canvas.delete(ALL)
        if img:
            try: img = self.canvas.create_image(0, 0, anchor=NW, image=img)
            except:                
                img = ImageTk.PhotoImage(img)
                self.images.append(img) # If img is local it's not drawing
                self.canvas.create_image(0, 0, anchor=NW, image=img)

    def canvas_gird(self, offsetx, offsety, cols, rows):
        offsetx = offsetx*self.dpioffset+self.dpioffset
        offsety = offsety*self.dpioffset+self.dpioffset        
        maxx = cols*offsetx
        maxy = rows*offsety
        self.canvas.configure(width=maxx-self.advoffset, height=maxy-self.advoffset)
        for x in range(0, cols+1):
            offx = x*offsetx
            #if offx > maxx: offx = maxx
            self.canvas.create_line(offx, 0, offx, maxy)
        for y in range(0, rows+1):
            offy = y*offsety
            #if offy > maxy: offy = maxy
            print offy
            self.canvas.create_line(0, offy, maxx, offy)

    def canvas_save(self):
        self.canvas.update()
        #for i in ['winfo_containing', 'winfo_fpixels', 'winfo_pixels',
        #             'winfo_screenheight', 'winfo_screenmmheight', 'winfo_screenmmwidth', 'winfo_screenwidth']:
        #    print "==="
        #    print i
        #    e = getattr(self.canvas, i)
        #    try: print e()
        #    except Exception, er: print er
        #    print "==="
        image = os.tmpfile()
        image.write(self.canvas.postscript(colormode='color'))
        image.seek(0)
        img = Image.open(image)
        path = asksaveasfilename()
        if not path or path is '': return
        img.save(path, dpi=(72, 72))

    def __fill(self, master):
         self.frame = self.main.load_xml(master, "imgcreator")
         self.canvas = self.main.fields["creator_canvas"]
         for param in ['width', 'height', 'rows', 'cols']:
             try: setattr(self, param, self.main.fields["creator_image_"+param])
             except: print "Failed param set"
         for value in [self.width, self.height, self.rows, self.cols]: 
             #this may be realized with eval in previous loop but many evals not good.
             value.bind("<KeyPress>", self.check_field)
         self.girdbutton = self.main.fields["creator_gird_create"]
         self.girdbutton.config(command=self.create_gird)
         self.savebutton = self.main.fields["creator_canvas_save"]
         self.savebutton.config(command=self.canvas_save)

    def check_field(self, event):
        self.check_value(event.widget)

    def check_value(self, widget):
        get = widget.get()
        widget.config(bg="#ff4400")
        try: int(get)
        except: return
        if len(widget.get()) > 0:
            widget.config(bg="white")
            return True

    def create_gird(self):
        for value in [self.width, self.height, self.rows, self.cols]:
            if not self.check_value(value.objects["entry"]):
                return
        self.canvas_draw(int(self.width.get())*int(self.cols.get()), int(self.height.get())*int(self.rows.get()))
        self.canvas_gird(int(self.width.get()), int(self.height.get()), 
                                 int(self.cols.get()), int(self.rows.get()))
