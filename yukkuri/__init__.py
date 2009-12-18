#import math
#import pyglet
#import random

import layer
import layer.audio
import layer.colors
import layer.command
import layer.const
import layer.load
import layer.coll
import layer.task
import layer.keybind
import layer.gl

from layer.sprite import Sprite, Group, OrderedGroup, batch, Batch
from layer.entity import Entity, EntityGroup
from misc import *
from entity import *

class World(object):
    _state = "main"

    def __init__(self, window):
        self.window = window        
        self.title = Sprite(filename="title.png")
        self.diffuse_batch = Batch(z=100)
        self.sprites_batch = Batch(z=-1)
        self.ui_batch = Batch(z=1)                 
        self.width = window.width
        self.height = window.height
        self.x = 0
        self.y = 0
        self.timer = Timer();
        self.group = OrderedGroup(1)
        self.sprfloor = OrderedGroup(1, parent=self.group)
        self.spritems = OrderedGroup(2, parent=self.group)
        self.sprites = OrderedGroup(3, parent=self.group)
        self.ents = EntityGroup()
        self.items = EntityGroup()
        self.time = TimeOfDay(self, window)
        self.player = Player(self)
        self.ents.add(self.player)
        self.chances = EntityDef.FindAll("meeting")
        #self.map = Map.Find("Main")
        #self.main_bg = Sprite(
            #z=-1, image=self.map.map_texture, batch=self.sprites_batch)        
        self.blocksize = layer.const.BLOCK_SIZE
        self.pathing = [[0 for y in range(self.height / self.blocksize)]
                        for x in range(self.width / self.blocksize)]
        #self.open = self.map.open_grid(self.blocksize)
        self.diffsprites = []
        self.floorsprites = {"x":{}, "y":{}}
        self.floor = []
        self._diffuse_accum = False
        self._action = False
        self.task = layer.task.Task(self.update)
        self.spawner = Spawner(self)
        self.night = False            
        for x, l in enumerate(self.pathing):
            sprs = []
            for y, v in enumerate(l):
                spr = Sprite(batch=self.diffuse_batch, filename="white")
                spr.width = self.blocksize
                spr.height = self.blocksize
                spr.x = x * self.blocksize
                spr.y = y * self.blocksize
                spr.color = (255, 0, 0)
                spr.opacity = 0
                sprs.append(spr)
            self.diffsprites.append(sprs)
        for x in range(int(-(self.width*0.3)/self.blocksize), int(self.width*1.3/self.blocksize)):
            sprs = []
            for y in range(int(-(self.height*0.3)/self.blocksize), int(self.height*1.3/self.blocksize)):
                num = str(random.randint(1, 8)) 
                spr = Sprite(batch=self.sprites_batch, group=self.sprfloor, filename="grass"+num+".png")
                spr.width = self.blocksize
                spr.height = self.blocksize
                spr.x = x * self.blocksize
                spr.y = y * self.blocksize
                for i in ("x", "y"):
                    if i == "x":
                        g = spr.x
                    else:
                        g = spr.y
                    if not self.floorsprites[i].has_key(g):
                        self.floorsprites[i][g] = []
                    self.floorsprites[i][g].append(spr)                
                sprs.append(spr)
            self.floor.append(sprs)

    #def gameover(self):
        #self.gameover = GameOver(self.player, self.window)
        #self.disable()

    def _get_state(self):
        return self._state

    def _set_state(self, state):
        layer.keybind.pop("main")        
        try: self.window.batches.remove(self.sprites_batch)
        except ValueError: pass
        self._state = state.lower()
        if self._state == "main":            
            layer.keybind.push("main")
            self.window.batches.append(self.sprites_batch)
            self.task.paused = False
            self.ents.enable()
            self.items.enable()

    state = property(_get_state, _set_state)

    def update(self, dt):
        self.timer.update(dt)
        timer = self.timer.get()
        self.spawner.spawn()                                        
        if self.time.time > layer.const.EVENING:
            pass
        elif self.time.time > layer.const.MORNING:
            if self.night:
                self.night = False        
        if timer < 1:
            #if not self._diffuse_accum: self._diffuse_accum = True
            if not self._action: self._action = True
        if  self._action and timer > 1:
            self._action = False
            self.action()
        #if self._diffuse_accum and timer > layer.const.REFLOW_TIMER:
            #self._diffuse_accum = False
            #self.flow()
        if timer > layer.const.MAX_TIMER:
            self.timer.clear()
        self.time.update(dt)
        self.player.inventory.update(dt)

    def debug_diffusion(self, enable=None):
        if enable is None:
            return self.diffuse_batch in self.window.batches
        elif enable and self.diffuse_batch not in self.window.batches:
            self.window.batches.append(self.diffuse_batch)
        elif not enable and self.diffuse_batch in self.window.batches:
            self.window.batches.remove(self.diffuse_batch)

    def action(self):
        for ent in self.ents:
            if ent.sprite.color != ent.dfn.color:
                    ent.sprite.color = ent.dfn.color
            if not isinstance(ent, Bot): continue                                                    
            if ent.attacked:                                
                if ent.attacked.hp <= 0 or dist(ent, ent.attacked) >= 1000:
                    ent.attacked = None
                else:
                    if ent.hp <= ent.max_hp*0.3:
                        ent.run_away(ent.attacked.x, ent.attacked.y)
                    else:
                        dst = dist(ent, ent.attacked)
                        if dst < 100 * ent.sprite.scale:                            
                            ent.attack(ent.attacked)
                        else:                            
                            ent.force_go_to = (ent.attacked.x, ent.attacked.y)

    def flow(self):        
        old_path = self.pathing
        width = int(self.height / self.blocksize)
        height = int(self.height / self.blocksize)
        self.pathing = [[0.0 for y in range(height)] for x in xrange(width)]
        self.tasty = {}
        self.offensive = {}
        self.blocking = {}
        def p(x, y):
            if (x, y) in self.blocking:
                return 0.0
            #elif not self.open[x][y]:
            #    return 0.0
            elif (x, y) in self.tasty:
                return self.tasty[(x, y)].dfn.tasty
            elif (x, y) in self.offensive:
                return 20.0
            else:
                vleft = (x > 0) and old_path[x - 1][y] or 0
                vright = (x < width - 1) and old_path[x + 1][y] or 0
                vup = (y < height - 1) and old_path[x][y + 1] or 0
                vdown = (y > 0) and old_path[x][y - 1] or 0
                v = (vleft + vright + vup + vdown) / 4.0
                return v
        for x in xrange(width):
            for y in xrange(height):
                self.pathing[x][y] = p(x, y)
                if self.debug_diffusion():
                    self.diffsprites[x][y].opacity = (
                        self.pathing[x][y] / 60.0) * 255

    def valid_pos(self, ox, oy, nx, ny):
        dx = nx - ox
        dy = ny - oy
        err = 0
        for ent in self.ents:
            if ox == ent.x and oy == ent.y: continue
            else:
                sx = ent.x  - (ent.width * 0.70)
                sy = ent.y - (ent.height * 0.70)
                if ( sx <= nx and nx <= sx + ent.width*1.4) and (sy <= ny and ny <= sy + ent.height*1.4):
                    return ox, oy                
        return nx, ny

    def interact(self):
        pass
    
    def give_item(self, dfn, slot=0):
        item = Item(dfn, self)
        self.player.inventory.add(item)

    def pickup_drop(self, slot):
        if self.player.inventory[slot]:
            self.player.drop(slot)
            return
        nearest = None
        nearest_d = 1e9
        for ent in self.ents:
            if not ent.dfn.takeable: continue
            dx = self.player.x - ent.x
            dy = self.player.y - ent.y
            d = dx * dx + dy * dy
            if d < nearest_d:
                nearest = ent
                nearest_d = d
        if nearest_d <= layer.const.BLOCK_SIZE * layer.const.BLOCK_SIZE:
            self.player.pickup(nearest, slot)

    def seek(self, ent, confusion=0):
        x, y = ent.block
        confusion /= 2
        values = []
        for dx in [0, -1, 1]:
            for dy in [0, -1, 1]:
                try: values.append([self.pathing[x + dx][y + dy], dx, dy])
                except IndexError: pass
                else:
                    values[-1][2] *= 1 + random.uniform(-confusion, confusion)
        if values:
            best, dx, dy = max(values)
            if best > 0.01:
                return (dx, dy)
        return (0, 0)

    def enable(self):
        self.title.batch = None
        layer.keybind.pop("menu")
        layer.command.attr_command(self, "state")
        layer.command.attr_command(self.player, "up")        
        layer.command.attr_command(self.player, "down")
        layer.command.attr_command(self.player, "left")
        layer.command.attr_command(self.player, "right")
        layer.command.command(
            self.debug_diffusion, declspec=[layer.command.cmd_bool])
        layer.command.command(self.interact)
        layer.command.command(self.player.choose)
        layer.command.command(self.player.show_stats)
        layer.command.command(self.player.attack)
        layer.command.command(self.player.rape)
        layer.command.command(self.player.use)
        layer.command.command(self.player.talk)        
        layer.command.command(self.pickup_drop, "pickup", declspec=[int])
        layer.command.command(self.player.drop, declspec=[int])
        layer.command.command(self.player.cmove, declspec=[int])
        #layer.command.command(self.give_item, declspec=[EntityDef.Find, int])
        layer.command.uncommand(self.enable)
        layer.command.command(self.disable)
        self.task.enable()
        self.ents.enable()
        self.items.enable()
        self.ents.window = self.window
        self.items.window = self.window
        self.state = 'main'
        self.window.batches.append(self.ui_batch)
        self.player.move_camera()

    def disable(self):
        self.player.restore_camera()
        self.title.batch = batch
        layer.keybind.push("menu")
        layer.command.command(self.enable)
        layer.command.uncommand(self.disable)
        layer.command.uncommand("up")
        layer.command.uncommand("down")
        layer.command.uncommand("left")
        layer.command.uncommand("right")        
        layer.command.uncommand(self.interact)
        layer.command.uncommand(self.player.cmove)
        layer.command.uncommand(self.player.choose)        
        layer.command.uncommand(self.player.show_stats)
        layer.command.uncommand(self.player.attack)
        layer.command.uncommand(self.player.rape)
        layer.command.uncommand(self.player.use)
        layer.command.uncommand(self.player.talk)
        #layer.command.uncommand(self.pickup_drop)        
        #layer.command.uncommand(self.player.drop)
        #layer.command.uncommand(self.give_item)        
        #layer.command.uncommand(self.debug_diffusion)
        self.task.disable()
        self.ents.disable()
        self.items.disable()
        self.ents.window = None
        self.items.window = None
        for b in (self.sprites_batch, self.ui_batch):
            try: self.window.batches.remove(b)
            except ValueError: pass

    def destroy(self):
        self.disable()
        self.ents.destroy()
        self.items.destroy()

class Spawner(object):

    def __init__(self, world):        
        self.world = world
        self.count = 0
        self.edibles = 0

    def spawn(self):
        if self.edibles < int(layer.const.MAX_EDIBLES):
            x, y = self.getxy()
            b = Item(EntityDef.Find("flowers"), self.world)
            b.sprite.x, b.sprite.y = b.x, b.y = x, y              
            self.edibles += 1
        if self.count < int(layer.const.MAX_SPAWN):
            x, y = self.getxy()
            b = Bot(self.world, x, y)
            self.world.ents.add(b)
            b.sprite.batch = self.world.sprites_batch
            self.count += 1

    def getxy(self):
        valsx = [
                   random.randint(int(self.world.player.x-self.world.width*2.0), 
                                           int(self.world.player.x-self.world.width * 0.5)),
                   random.randint(int(self.world.player.x+self.world.width*0.5), 
                                           int(self.world.player.x+self.world.width*2.0))]
        valsy = [
                   random.randint(int(self.world.player.y-self.world.height*2.0), 
                                           int(self.world.player.y-self.world.height*0.5)),
                   random.randint(int(self.world.player.y+self.world.height*0.5), 
                                           int(self.world.player.y+self.world.height*2.0))]
        x = valsx[random.randint(0, 1)]
        y = valsy[random.randint(0, 1)]
        return (x, y)

class GameOver(object):
    def __init__(self, player, window):
        text = """<center><font color=white>
<font color=white>You:<br>\
<font color=white>Survived %(days)d days<br>\
<font color=white>Earned $%(money)d<br>\
<font color=white>Killed %(killed)d rabbits<br>\
<font color=white>Cured %(cured)d rabbits<br>\
<font color=white>Grew %(grown)d plants</font></center>"""
        text = text % dict(
            money=player.inventory.money,
            days=player.days,
            killed=player.killed,
            cured=player.saved,
            grown=player.grown)
        self.batch = Batch(z=2)
        self.window = window
        self.bg = Sprite(filename="gameover.png", batch=self.batch,
                     group=self.batch.Group(1))
        self.label = pyglet.text.HTMLLabel(            
            text, batch=self.batch,
            y=window.height * 0.6,
            multiline=True,
            width=window.width / 2,
            group=self.batch.Group(2))
        self.label.font_size = 20
        window.batches.append(self.batch)

    def delete(self):
        self.window.batches.remove(self.batch)
        self.bg.delete()
        self.label.delete()
            
def init(argv):
    window = layer.init(
        "Yukkuri", _("Yukkuri"), argv, size=(800, 600))
    return window

def main(window, argv):
    layer.command.process_argv(argv)
    layer.keybind.push("menu")
    layer.keybind.push("first")
    world = World(window)
    layer.command.command(world.enable)
    #layer.audio.music.play("town.ogg")
    layer.run(window)
