import math
import pyglet
import random
import re

import layer
#import layer.audio
#import layer.colors
#import layer.command
#import layer.const
#import layer.load
#import layer.coll
#import layer.task
#import layer.keybind
#import layer.gl
import layer.output

from layer.sprite import Sprite, Group, OrderedGroup, batch, Batch
from layer.entity import Entity, EntityGroup

def dist(a, b):    
    return math.sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))

def prob(**vars): # keys - number; values - probability(%)
    rnd = random.random()
    min, max = 0.0, 0.0      
    for i in vars.keys():
        vars[i] = float(vars[i]) / 100
        max += vars[i]
        if min < rnd <= max:
            return i

def text_parce(text, **vars):
    m = re.findall(ur"%(\w+)%", text, re.M)
    if len(m):
        for i in m:				
            r = re.compile(ur"%%%s%%" % i)
        if vars.has_key(i):
            text = re.sub(r, str(vars[i]), text)
    return text

class Timer(object):
    def __init__(self):
        self.timer = 0
        self.work = False
        self.enable()
    def update(self, dt):
        if self.work:
            self.timer += dt
    def __get__(self):
        return self.timer
    def get(self):
        return self.timer
    def clear(self):
        self.timer = 0 
    def disable(self):
        self.work = False
    def enable(self):
        self.work = True

class EntGroup(list): #Move into layer. This copy EntityGroup methods, it's not good
    """This class - simple group manager"""
    
    def __init__(self, leader=None):    
        self.leader=leader
        self.add(leader)
    def add(self, entity):
        try: index = self.index(entity)
        except ValueError:
            self.append(entity)
            return len(self) - 1
        else:
            return index
    def kill(self, entity):
        try: index = self.index(entity)
        except ValueError: return -1
        else:
            self.pop(index)
            return index
    remove = kill
    def destroy(self):
        self.leader = None
        for entity in list(self):
            entity.kill()

class Family(EntGroup): #think about compare with DialogueMeta. 
    
    def __init__(self, starter):
        super(Family, self).__init__(starter)
        self.world = starter.world
        self.enemy = None

    def join(self, obj):
        self.add(obj)
        return self

    def leave(self, obj, killer=None):
        if self.leader == obj:
            self.leader_dead(killer)
        self.remove(obj)
        obj.party = None

    def exp(self, exp):
        for member in list(self):
            member.exp += exp*member.level

    def family_jobs(self, ent):
        # this method into tick of bots. here will be:        
        # something else?
        if len(ent.party) < 2:
            self.leave(ent)
            return
        elif len(ent.party) > 2:
            if self.leader is not ent and ent.level >= layer.const.rape_level:
                self.leave(ent)
                if random.randint(0, 100) > layer.const.FAMILY:
                    ent.wantparty = True
        if not ent.hungry or not ent.attacked: # self is first!
            self.follow_leader(ent)
            if ent.attacked and ent.attacked not in list(self):
                self.enemy = ent.attacked
            if self.enemy:
                if self.enemy.hp <= 0:
                    self.enemy = None
                if self.enemy:
                    ent.attacked = self.enemy

    def follow_leader(self, ent):        
        if ent is self.leader or ent is self.world.player: 
            return
        if dist(self.leader, ent) > 500*ent.sprite.scale:
            ent.force_go_to = (random.randint(int(self.leader.x - 50), int(self.leader.x + 50)),
                                         random.randint(int(self.leader.y - 50), int(self.leader.y + 50)))

    def leader_dead(self, killer=None):
        #self.remove(self.leader)
        if len(list(self)) > 1:
            if not killer or killer not in list(self):             
                self.leader = self[0]
            else:
                self.leader = killer
        else:
            self[0].family = None
            self.remove(self[0])

class DialogueBlock(layer.parse.ParseObject):
    actor = 0
    time = 2
    text = ""
    chooser = "first"

class DialogueChoice(layer.parse.ParseObject):
    text = ""
    command = False
    falseframe = None

class DialogueDef(layer.parse.ParseObject):
    load_spec = "*.dialogue"
    actors = {}
    blocks = []

class DialogueMeta(object):
    
    ################
    ## If actor dead dialogue crashes. Repair!    
    
    def __init__(self, starter):
        self.starter = starter        
        self.group = OrderedGroup(20, parent=self.starter.world.group)
        self.boxgroup = OrderedGroup(3, parent=self.group)        
        self.textgroup = OrderedGroup(4, parent=self.group)
        self.aboxgroup = OrderedGroup(10, parent=self.group)
        self.atextgroup = OrderedGroup(12, parent=self.group)
        self.roles = ["first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth"]
        self.actors = {}
        self.choices = []
        self.choose = 0
        self.add_member(starter, "first")
        self.next = None
        self.timerstatus = None
        self.timer = 0

    # __getitem__ __setitem__ maybe?
    def add_member(self, member, role):
        self.actors[role] = member

    def remove_member(self, role):
        del self.actors[role]

    def next_frame(self):          
        if self.next == None or self.next == "last" or not self.dfn or self.next >= len(self.dfn.blocks):
            self.timer = 0
            self.timerstatus = None
            self.destroy()
        else:
           self.__set_frame(self.next)

    def set_dfn(self, dfn, frame=0):
        self.dfn = DialogueDef.Find(dfn)
        self.next = frame
        self.next_frame()

    def __set_frame(self, frame):
        block = self.dfn.blocks[frame]
        if self.actors.has_key(block.actor):
            actor = self.actors[block.actor]
        else:
            self.next = "leave"
            self.next_frame()
            return
        text = block.text
        if hasattr(block, "nextframe"):
            self.next = block.nextframe
        else: 
            self.next = frame+1
        if hasattr(block, "command"):
            if hasattr(block, "falseframe") and hasattr(block, "trueframe"):
                try: action = eval(block.command)
                except: action = False
                if not action:
                    self.next = block.falseframe
                else:
                    self.next = block.trueframe
            else:
                if not text.find("%s"):
                    try: eval(block.command)
                    except Ecteption, e: print_d(e) 
                else:
                    try: replace = eval(block.command)
                    except Ecteption, e: print_d(e)
                    else: text = text.replace("%s", str(replace))
        self.__show_text(actor, text, actor.alabel)
        if hasattr(block, "choices"):
            self.make_choices(block.chooser, block.choices)
        self.timer = 0
        self.timerstatus = (block.time or 5)

    def __show_text(self, actor, text, label, dx=0, dy=0):
        for a in self.actors.keys():
            self.actors[a].box.group = self.boxgroup
            for i in ("text", "x", "y", "color"):
                setattr(self.actors[a].label, i, eval("self.actors[a].alabel."+i))
            self.actors[a].alabel.color = (0, 0, 0, 0)
        label.text = "<center>%s</center>" % (text)
        label.font_size = 12
        actor.box.width = actor.width
        actor.box.height = actor.height
        actor.box.image.anchor_x = actor.width / 2
        actor.box.image.anchor_y = actor.height / 2
        actor.box.x = actor.owner.x
        actor.box.y = actor.owner.y + actor.owner.height/2 + actor.box.height/2
        label.x, label.y = actor.box.x+dx, actor.box.y+dy
        if label.content_width > actor.box.width:
            actor.box.width = label.content_width + 10
            label.x -= actor.box.image.anchor_x*2 + 5
        if label.content_height > actor.box.height:
            actor.box.height = label.content_height + 10
            label.y += actor.box.image.anchor_y*2 - 15
        actor.box.group = self.aboxgroup
        actor.box.opacity = 255
        label.color = (0, 0, 0, 255)
        actor.label.color = (0, 0, 0, 0)

    def make_choices(self, role, choices):
        actor = self.actors[role]
        oldy = 0
        for c in choices:
            c.label = self.choice_label(actor)
            self.choices.append(c)
            self.__show_text(actor, c.text, c.label, 0, oldy)
            oldy -= c.label.content_height
        actor.box.height = oldy*1.4
        self.cmove(0)
        layer.keybind.push("Dialogue")
        layer.keybind.pop("Main")

    def choice_label(self, actor):
        label = pyglet.text.HTMLLabel(batch = actor.world.sprites_batch,
            multiline=True,  group=self.atextgroup, anchor_x="center", 
            anchor_y="center", height=actor.box.height, width = actor.box.width, text = "")
        return label

    def cmove(self, ind):
        if ind + self.choose > len(self.choices) - 1:
            ind = 0
        elif ind + self.choose < 0:
            ind = len(self.choices)  - 1
        else: 
            ind = self.choose + ind
        self.choices[self.choose].label.bold = False
        self.choices[ind].label.bold = True
        self.choose = ind

    def make_choose(self, actor):
        block = self.choices[self.choose]
        text = block.label.text
        action = eval(str(block.command))
        self.next = None
        if hasattr(block, "falseframe") and not action:
            self.next = block.falseframe
        elif hasattr(block, "trueframe") and action:
            self.next = block.trueframe
        elif hasattr(block, "nextframe"):
            self.next = block.nextframe
        for c in self.choices:
            c.label.delete()
        del self.choices[:]
        self.choose = 0
        self.__show_text(actor, text, actor.alabel)
        self.timer = 0
        self.timerstatus = 2
        layer.keybind.push("Main")
        layer.keybind.pop("Dialogue")

    def destroy(self):
        for actor in self.actors.keys():
            self.actors[actor].leave()

class Dialogue(object):
    
    def __init__(self, owner):
        self.owner = owner
        self.world = owner.world
        self.started = None        
        
        
    def start(self, *objects):
        if self.started:
            return
        self.started = DialogueMeta(self)
        self.set_role(self.started.roles[0], self.started)
        if len(objects) > 0:
            for i in range(0, len(objects)):
               if objects[i].dialogue.started or objects[i].blocked:
                   return False
               objects[i].dialogue.set_role(self.started.roles[i+1], self.started)
        return True        

    def set_role(self, role, meta):
        if self.started and self.started is not meta:
            return
        self.role = role
        self.started = meta
        meta.add_member(self, role)
        self.create_buble()

    def create_buble(self):
        self.box = Sprite(filename="dialogue.png", batch = self.world.sprites_batch,
                                  group=self.started.boxgroup)
        self.box.opacity = 0
        self.width = self.box.width
        self.height = self.box.height
        self.label = pyglet.text.HTMLLabel(batch = self.world.sprites_batch,
            multiline=True,  group=self.started.textgroup, anchor_x="center", 
            anchor_y="center", height=self.box.height, width=self.box.width, text = "")            
        self.alabel = pyglet.text.HTMLLabel(batch = self.world.sprites_batch,
            multiline=True,  group=self.started.atextgroup, anchor_x="center", 
            anchor_y="center", height=self.box.height, width = self.box.width, text = "")
            # FUCK, I think that I'm curve and can't make Groups work properly
        self.label.content_valign = "center"
        self.label.color = (0, 0, 0, 0)
        self.alabel.content_valign = "center"
        self.alabel.color = (0, 0, 0, 0)

    def leave(self):
        if self.started:
            self.owner.blocked = False
            self.started.remove_member(self.role)
            self.destroy()

    def destroy(self):
        self.started = None
        self.role = None
        if self.box:
            self.box.delete()
        if self.label:
            self.label.delete()
        if self.alabel:
            self.alabel.delete()

class TimeOfDay(object):
    def __init__(self, world, window):
        self.world = world        
        self.group = OrderedGroup(20)
        self.dark = Sprite(
            filename="white", group=OrderedGroup(0, parent=self.group),
            batch=world.ui_batch)
        self.light = Sprite(
            filename="white", group=OrderedGroup(1, parent=self.group),
            batch=world.ui_batch,
            blend_src=layer.gl.GL_SRC_ALPHA,
            blend_dest=layer.gl.GL_ONE)
        self.dark.x = self.light.x = 0
        self.dark.y = self.light.y = 0
        self.dark.width = self.light.width = window.width
        self.dark.height = self.light.height = window.height
        self.dark.color = (0, 0, 0)
        self.light.color = (255, 255, 0)
        self.time = 10
        self.light.opacity = 0
        self.days_update = False

    def update(self, dt):
        hours = 24 * dt / layer.const.DAY_LENGTH
        self.time = (self.time + hours) % 24
        if self.time > 22 or self.time < 2:            
            self.dark.opacity = 128
            if int(self.time) == 0:
                if not self.days_update:
                    for i in self.world.ents:
                        i.days += 1
                        if i.growth: i.days_to_grow -= 1
                self.days_update = True
            else:
                self.days_update = False
        elif self.time > 18:
            p = (self.time - 18) / 4.0
            self.dark.opacity4 = [128 * p,
                                  min(128, 256 * p),
                                  min(128, 256 * p),
                                  128 * p,
                                  ]
        elif self.time < 6:
            p = 1 - (self.time - 2) / 4.0
            self.dark.opacity4 = [min(128, 256 * p),
                                  128 * p,
                                  128 * p,
                                  min(128, 256 * p),
                                  ]
        else:
            self.dark.opacity = 0
        if 4 < self.time < 8:
            # light fades black to yellow, additive, but also fades out
            self.light.opacity = 255 * (self.time - 4) / 4
            p = (self.time - 4) / 4.0
            ceast = (255 * p, 255 * p, 0)
            cwest = (512 * p, 512 * p, 0)
            self.light.color4 = [ceast, cwest, cwest, ceast]
            self.light.opacity = 64 * (1 - p)
        else:
            self.light.opacity = 0

class Interactable(layer.parse.ParseObject): #This for map points
    command = ""
    radius = 10
    x = 0
    y = 0
    name = ""

class Map(layer.parse.ParseObject):
    load_spec = "*.map"
    interactables = []
    prompt = ""
    _shelves_ = ["map", "collision", "interactables", "width", "height", "prompt"]

    def find(self, name):
        for i in self.interactables:
            if i.name.lower() == name.lower():
                return i.x, i.y
        return 0, 0
    
    def __setstate__(self, d):
        super(Map, self).__setstate__(d)
        self.map_texture = layer.load.image(self.map) 
        coll = layer.load.image(self.collision)
        texels = coll.image_data.get_data("RGBA", coll.width * 4)
        self.speedmap = {}
        for i in range(0, len(texels), 4):
            color = texels[i:i+4]
            v = min(color) / 255.0
            if v < 1:
                x = (i / 4) % self.width
                y = (i / 4) / self.width
                self.speedmap[(x, y)] = v

    def interact(self, x, y):
        for i in self.interactables:
            dx = i.x - x
            dy = i.y - y
            if dx * dx + dy * dy < i.radius * i.radius and i.command:
                layer.command.run(i.command)

    def prompt(self, x, y):
        for i in self.interactables:
            dx = i.x - x
            dy = i.y - y
            if dx * dx + dy * dy < i.radius * i.radius and i.command:
                return i.prompt
        return ""

    def open_grid(self, blocksize):
        grid = []
        for x in range(0, self.width, blocksize):
            l = []
            grid.append(l)
            for y in range(0, self.height, blocksize):
                for dx in range(x, x + blocksize):
                    for dy in range(y, y + blocksize):
                        if self.speed(dx, dy) < 0.1:
                            l.append(False)
                            break
                    if len(l) > y / blocksize: break
                else:
                    l.append(True)
        return grid

    def speed(self, x, y):
        return self.speedmap.get((int(x), int(y)), 1)

class Inventory(object):
         
    def __init__(self, player, length, window):
        self.group = OrderedGroup(100)
        self.boxgroup = OrderedGroup(0, parent=self.group)
        self.bargroup = OrderedGroup(1, parent=self.group)
        self.sprgroup = OrderedGroup(2, parent=self.group)
        self.window = window
        self.player = player
        self.items = []
        self.stats = []
        self.statssw = True
        self.batch = batch = player.world.ui_batch
        self.timebg = Sprite(group=self.boxgroup, batch=self.batch,
                         filename="time.png")
        self.timebg.x = self.window.width - self.timebg.width
        self.timebg.y = self.window.height - self.timebg.height
        self.items.append(self.timebg)
        self.create_stats()
        self.timeofday = {}
        self.partyobj = []
        label = pyglet.text.Label(text= "Player", group=self.sprgroup,  
                batch=self.batch, font_size = 14, color = (200, 30, 30, 255))
        label.x = self.player.world.width/2 - label.content_width/2 + 10
        label.y = self.player.world.height/2 - self.player.height
        self.items.append(label)
        for tod in ['twilight', 'morning', 'noon', 'afternoon', 'evening', 'midnight']:
            spr = Sprite(filename=tod + ".png", group=self.sprgroup, batch=self.batch)            
            spr.opacity = 0
            spr.x = self.window.width - self.timebg.width + 43
            spr.y = self.window.height - self.timebg.height + 18
            self.items.append(spr)
            self.timeofday[tod] = spr            
        self.time = player.world.time
        self.rape = None
        #for i in xrange(length):
            #self.items.append(None)

    def party(self):
        party = self.player.party
        if not party:
            self.partyhide()
            return
        if len(self.partyobj) <> len(party):
            self.partyhide()
            leader = Sprite(group=self.boxgroup, batch=self.batch, 
                              filename=party.leader.imgmini)                              
            leader.scale = 0.7
            self.partyobj.append(leader)
            self.items.append(leader)
            oldx = leader.width 
            for m in party:                
                if m == party.leader: continue
                member = Sprite(group=self.boxgroup, batch=self.batch, 
                              filename=m.imgmini)                
                member.scale = 0.7
                member.x = member.width*party.index(m)
                self.partyobj.append(member)
                self.items.append(member)

    def partyhide(self):
        if self.player.party and len(self.player.party) < 2:
            self.player.party.leave(self.player)
        for i in self.partyobj:
            i.delete()
        self.partyobj = []

    def rapeshow(self):
        if not self.rape:
            self.rape = pyglet.text.Label(bold=True, text= "It's Rape time", 
                group=self.sprgroup, batch=self.batch, font_size = 13, color = (255, 0, 0, 255))
            self.rape.x = self.window.width - self.rape.content_width*1.5
            self.rape.y =  self.window.height - self.timebg.height - self.rape.content_height
            self.items.append(self.rape)

    def rapehide(self):
        if self.rape:
            self.rape.delete()
            self.rape = None

    def create_stats(self):
        #rewrite. longfunction is looooooooooong
        statsbg = Sprite(group=self.boxgroup, batch=self.batch,
                         filename="state_back.png")        
        statsbg.y = self.window.height - statsbg.height
        img = Sprite(filename=self.player.imgmini, group=self.sprgroup, batch=self.batch)
        img.x = img.width  / 8
        img.y = self.window.height - img.height
        self.lvl = pyglet.text.Label(
            text= "", group=self.sprgroup, batch=self.batch, font_size = 13, color = (0, 0, 0, 255), 
            x = int(math.ceil(img.x*7 + 20)), y = int(math.ceil(img.y + img.width/2 - 5)))
        self.days = pyglet.text.Label(
            text= "", group=self.sprgroup, batch=self.batch, font_size = 13, 
            x = self.lvl.x, y = self.lvl.y - 25, color = (0, 0, 0, 255))
        for i in (statsbg, img, self.lvl, self.days):
            self.stats.append(i)
            self.items.append(i)
        for i in ("hp", "fed", "exp"):
            lbl = pyglet.text.Label(text= "", group=self.sprgroup, batch=self.batch, font_size = 11)
            if i == "hp":
                lbl.x = int(math.ceil(img.x*2 + 5))
                lbl.y = int(math.ceil(self.window.height - img.height*1.4))
            else:
                lbl.x = oldbarx + 5
                lbl.y = oldbary - 18           
            oldlblx, oldlbly = lbl.x, lbl.y
            lbl.color = (0,0,0, 255)
            setattr(self, i+"lbl", lbl)
            bar =  Sprite(filename="bar.png", group=self.bargroup, batch=self.batch)
            if i == "hp": bar.color = (155, 0, 0)
            elif i == "fed": bar.color = (0, 0, 255)
            elif i == "exp": bar.color = (10, 170, 30)
            else: bar.color = (155, 155, 155)
            bar.x, bar.y = oldlblx - 5, oldlbly - 13
            oldbarx, oldbary = bar.x, bar.y             
            setattr(self, i+"bar", bar)
            bg = Sprite(filename="bar_outline.png", group=self.sprgroup, batch=self.batch)
            bg.x, bg.y = oldbarx - 7, oldbary - 7            
            for g in (lbl, bar, bg):
                self.stats.append(g)
                self.items.append(g)
        for i in self.player.stats.keys():
            if "oldx" not in locals():
                oldx = self.fedlbl.x
            if "oldy" not in locals():
                oldy = self.expbar.y
            lbl = pyglet.text.Label(text= "", group=self.sprgroup, batch=self.batch, font_size = 11)
            lbl.color = (0,0,0, 255)
            lbl.x = oldx
            lbl.y = oldy - 15
            oldx, oldy = lbl.x, lbl.y  
            setattr(self, i+"lbl", lbl)
            self.stats.append(lbl)
            self.items.append(lbl)
        self.show_stats_text()
    
    def stats_cahnge(self):                
        if self.statssw:
            for i in self.stats:
                if isinstance(i, pyglet.text.Label):
                    i.text = ""
                else: i.opacity = 0  
        else:
            for i in self.stats:
                if not isinstance(i, pyglet.text.Label):                    
                    i.opacity = 255
            self.show_stats_text()

    def scale_bars(self):
        self.hpbar.width = 132.0*float(self.player.hp)/float(self.player.max_hp)
        self.fedbar.width = 132.0*float(self.player.fed)/float(self.player.max_fed)
        self.expbar.width = 132.0*float(self.player.exp)/float(self.player.max_exp)            

    def show_stats_text(self):
        self.lvl.text= "Level:\t"+str(self.player.level)
        self.days.text= "Days:\t"+str(self.player.days)
        self.hplbl.text = "HP:\t"+str(int(self.player.hp))+"/"+str(int(self.player.max_hp))
        self.fedlbl.text = "Fed:\t"+str(int(self.player.fed*100))+"/"+str(int(self.player.max_fed*100))
        self.explbl.text = "Exp:\t"+str(int(self.player.exp))+"/"+str(int(self.player.max_exp))
        for i in self.player.stats.keys():
            setattr(eval("self."+i+"lbl"), "text", i+":\t"+str(self.player.stats[i]))

    def take(self, item_name):
        for i, item in enumerate(self.items):
            if item.dfn.name.lower() == item_name.lower():
                self.items[i] = None
                item.kill()
                return True
        return False
    
    def __getitem__(self, i):
        return self.items[i]
            
    def __setitem__(self, i, item):
        self.items[i] = item

    def __delitem__(self, i):
        if self.items[i]:
            self.items[i].kill()
            self.items[i].sprite.delete()
            self[i] = None

    def add(self, item):
        for i in range(len(self.items)):
            if self.items[i] is None:
                self.player.pickup(item, i)
                break
        else:
            self.player.pickup(item, 0)

    def update(self, dt=None):
        if self.statssw:
            self.scale_bars()
            self.show_stats_text()
        
        if self.time.time > 23 or self.time.time < 2:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["midnight"].opacity = 255
        elif self.time.time < 5:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["twilight"].opacity = 255
        elif self.time.time < 10:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["morning"].opacity = 255
        elif self.time.time < 14:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["noon"].opacity = 255
        elif self.time.time < 18:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["afternoon"].opacity = 255
        elif self.time.time < 23:
            for sprite in self.timeofday.itervalues(): sprite.opacity = 0
            self.timeofday["evening"].opacity = 255

    def move(self, dx, dy):
        for item in self.items:
            item.x += dx
            item.y += dy
