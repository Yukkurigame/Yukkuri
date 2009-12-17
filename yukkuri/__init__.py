import math
import pyglet
import random

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
            #layer.audio.music.play("field.ogg")
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
            #if self.player in self.ents:
                #pass
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
        #if not (dx or dy):
            #return ox, oy
        #elif dx == 0 and oy < ny:
            #for y in range(int(oy), int(ny) + 2, 1):
                #if self.map.speed(ox, y) < 0.1:  break
                #ox, oy = ox, min(y, ny)
        #elif dx == 0 and oy > ny:
            #for y in range(int(oy), int(ny) - 2, -1):
                #if self.map.speed(ox, y) < 0.1:  break
                #ox, oy = ox, max(y, ny)
        #else:
            #derr = (dy / dx)
            #y = oy
            #if dx > 0: dx, fx = 1, min
            #else: dx, fx = -1, max
            #if dy > 0: dy, fy = 1, min
            #else: dy, fy = -1, max
            #for x in range(int(ox), int(nx) + 2 * dx, dx):
                #if self.map.speed(x, y) < 0.1: break
                #err += derr
                #ox, oy = fx(x, nx), fy(y, ny)
                #if abs(err) >= 0.5:
                    #y = y + dy
                    #err -= 1
        return nx, ny
        #return clamp(ox, 0, self.width), clamp(oy, 0, self.height)

    def interact(self):
        pass
        #self.map.interact(self.player.x, self.player.y)
    
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
        #layer.command.command(self.player.change_action)
        #layer.command.command(self.player.action_do)
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

class Yukkuri(Entity):    

    def __init__(self, world):
        super(Yukkuri, self).__init__()
        self.world = world        
        self.x = world.width / 2
        self.y = world.height / 2
        self.images = self.dfn.down_anim
        self.sprite = Sprite(image=self.images[0], group=world.sprites,
                             batch=world.sprites_batch)
        self.sprite.color = (255, 255, 255)
        self.sprite.scale = 0.3
        self.bloodcolor = (self.dfn.bclrr, self.dfn.bclrg, self.dfn.bclrb)
        self.dialogue = Dialogue(self)
        self.attacked = None
        self.hungry = False
        self.days, self.distance, self.kills = 0, 0, 0
        self.width = self.dfn.width * self.sprite.scale
        self.height = self.dfn.height * self.sprite.scale        
        self.dx, self.dy = 0, 0
        self.level = 1
        self.stats = {"Str": 1, "Agi": 1, "Sta": 1, "Int": 1, "Def": 1}
        self.damage = self.dfn.damage
        self.hp = self.dfn.hp
        self.max_hp = self.level*self.hp
        self.max_fed = 1.0
        self.fed = 1.0
        self.exp = 20
        self.max_exp = self.level*100
        self.timer = self.world.timer
        self.timerstatus = False
        self.partyreject = []
        self.wantparty = False
        self.party = None
        self.tint = random.uniform(1, int(layer.const.MAX_TIMER))

    def levelup(self, up = 1):        
        self.level += up
        self.sprite.scale = math.log(self.level, 40)
        if self.sprite.scale < 0.3:
            self.sprite.scale = 0.3
        if self.sprite.scale > 1.2:
            self.sprite.scale = 1.2
        self.width = self.dfn.width * self.sprite.scale
        self.height = self.dfn.height * self.sprite.scale
        for i in self.stats.keys():
            self.stats[i] += 1
        hp = self.hp/self.max_hp
        self.exp = self.exp - self.max_exp
        self.max_hp *= self.level
        self.max_exp *= self.level
        self.hp = self.max_hp*hp

    def pickup(self, item, slot):
        self.drop(slot)
        self.world.ents.kill(item)
        self.inventory[slot] = item

    def drop(self, slot):
        item = self.inventory[slot]
        if item:
            item.x = self.x
            item.y = self.y
            item.sprite.group = self.world.sprites
            item.sprite.color = item.dfn.color
            item.sprite.batch = self.world.sprites_batch
            item.sprite.scale = (item.dfn.hp
                                 and (float(item.hp) / item.dfn.hp)
                                 or 1)
            self.world.ents.add(item)
        self.inventory[slot] = None

    def go_to(self, dt):
        if self.dx or self.dy:
            l = math.sqrt(self.dx * self.dx + self.dy * self.dy)
            speed = self.dfn.speed * self.fed * 2 #(self.dfn.speed * self.world.map.speed(self.x, self.y))
            if speed < 10:
                 speed = 20
            if speed == 0: #and self.force_go_to:
                speed = self.dfn.speed / 1
            self.dx = dt * speed * self.dx / l
            self.dy = dt * speed * self.dy / l
            nx = self.x + self.dx
            ny = self.y + self.dy            
            nx, ny = self.world.valid_pos(self.x, self.y, nx, ny)            
            if nx == self.x and ny == self.y:
                self.dx, self.dy = 0, 0
                if hasattr(self, "force_go_to") and self.force_go_to:
                    self.force_go_to = None
            else:
                distance = math.sqrt((nx - self.x) * (nx - self.x) + (ny - self.y) * (ny - self.y))
                self.distance += distance
                self.x =  nx
                self.y = ny
                if self.fed >= 0.01:
                    self.fed -= distance*0.0001/(self.stats["Agi"]+math.sqrt(self.stats["Sta"]))

    def attack(self, victim = None):
        if not victim:
            victim = self.closer(120, self.world.ents)
        if victim:
            self.dialogue_start(dfn="selftalking", frame=2)
            if self.fed >= 0.01:
                self.fed -= 0.001/(self.stats["Agi"]+math.sqrt(self.stats["Sta"]))
            victim.hit(self)

    def rape(self, victim = None):
        if not victim:
            victim = self.closer(120, self.world.ents)
        if victim:            
            self.dialogue_start(dfn="selftalking", frame=3)

    def hit(self, hiter):                        
        self.attacked = hiter
        self.sprite.color = (255, 0, 0)                
        dmg = hiter.damage+hiter.fed*hiter.stats["Str"]/(self.stats["Agi"]*self.stats["Def"])
        if dmg < 0.4:
            dmg = 0.4
        self.hp -=  dmg

    def dead(self):
        killer = None
        if self.attacked:
            killer = self.attacked
            killer.exp += self.max_hp*killer.stats["Int"]/killer.level
            killer.kills += 1
        dead = Item(EntityDef.Find("dead"), self.world)
        dead.hp = self.max_hp*self.fed/2        
        dead.sprite.scale = self.sprite.scale
        dead.blood = Sprite(filename="blood.png", group=self.world.spritems, 
                               batch=self.world.sprites_batch, x = self.x, y = self.y )
        if hasattr(self, "bloodcolor"):
            dead.blood.color = self.bloodcolor
        else:
            dead.blood.color = (155, 0, 0)
        dead.blood.scale = self.sprite.scale
        dead.sprite.x, dead.sprite.y = dead.x, dead.y = self.x, self.y
        if self.party and self.party.leader == self:
            self.party.leader_dead(killer)
        if self.dialogue.started:
            self.dialogue.destroy()
        self.kill()

    def eat(self, obj):
        #rewrite
        if obj.hp > 0:
            dmg = self.damage/self.fed
            obj.hp -= dmg
            if self.fed >= 0.8 * self.max_fed and self.hp < self.max_hp:
                    addhp = dmg/self.level
                    if self.hp + addhp > self.max_hp:
                        addhp = self.max_hp - self.hp
                    self.hp += addhp
            if self.fed >= self.max_fed:
                self.fed = self.max_fed
                if self.hp < self.max_hp:
                    addhp = dmg
                    if self.hp + addhp > self.max_hp:
                        addhp = self.max_hp - self.hp 
                    self.hp += addhp                    
            else:
                addfed = dmg*0.01*obj.nutritive/self.level
                if self.fed + addfed > self.max_fed:
                    addfed = self.max_fed - self.fed
                self.fed += addfed

    def take(self, obj):
        pass

    def invite(self, obj):        
        if obj.party:
            return False
        if obj.level > self.level + 2:            
            return False
        if self.party and self.party.leader is not self:
            return False
        return obj.join_party(self)

    def join_party(self, obj):
        if not obj.party:
            obj.party = Family(obj)
        self.party = obj.party.join(self)
        self.wantparty = False
        obj.wantparty = False
        return True

    def party_reject(self, obj):
        self.partyreject.append(obj)
        obj.partyreject.append(self)
        return True

    def dialogue_start(self, *obj, **kwargs):
        if not self.dialogue.start(*obj):
            return
        dfn, first = "main", 0        
        if kwargs.has_key("dfn"): dfn = kwargs["dfn"] 
        if kwargs.has_key("frame"): first = kwargs["frame"]
        self.dialogue.started.set_dfn(dfn, first)
        
    def dialogue_update(self, dt):
        if not self.dialogue.started or not self.dialogue.started.timerstatus:
            return
        self.dialogue.started.timer += dt
        if self.dialogue.started.timer > self.dialogue.started.timerstatus:
            self.dialogue.started.next_frame()

    def closer(self, dst, objects, reject=[]):
        def it(obj):
            if self is not obj:
                if not len(reject) or obj not in reject:
                    d = dist(self, obj)
                    if d <= dst*self.sprite.scale:
                        return {"obj": obj, "dist": d}
        def itms(x, y):            
            if  x["dist"] < y["dist"]: 
                return x
            else: 
                return y
        items = [f for f in map(it, objects) if f != None]
        if len(items) > 0:
            item = reduce(itms, items)
            return item["obj"]

    def update(self, dt):
        self.dialogue_update(dt)
        self.timer = self.world.timer.get()
        if self.exp >= self.max_exp:
            self.levelup()
        if self.fed < 0.7:
            if not self.hungry:
                self.dialogue_start(dfn="selftalk", frame=1)
            self.hungry = True        
        if self.fed > 0.95:
            self.hungry = False                
        if self.timer < 1:
            if not self.timerstatus: self.timerstatus = True
        if self.timerstatus and self.timer > self.tint:
            if self.party:
                self.party.family_jobs(self)            
            if self.fed >= 0.01:
                self.fed -= 0.01/self.level
            self.exp += 5*self.level

    def draw(self):        
        if not ((self.images == self.dfn.down_anim and self.dy < 0)
                or (self.images == self.dfn.up_anim and self.dy > 0)
                or (self.images == self.dfn.left_anim and self.dx < 0)
                or (self.images == self.dfn.right_anim and self.dx > 0)):
            if self.dy < 0: self.images = self.dfn.down_anim
            elif self.dy > 0: self.images = self.dfn.up_anim
            elif self.dx < 0: self.images = self.dfn.left_anim
            elif self.dx > 0: self.images = self.dfn.right_anim
        
        if self.dx or self.dy:
            imgidx = int(
                (self.distance / self.dfn.anim_distance) % len(self.images))
        else:
            imgidx = 0 
        img = self.images[imgidx]
        
        if img is not self.sprite.image:
            self.sprite.image = img
        if self.sprite.position != (self.x, self.y):
            self.sprite.position = (self.x, self.y)
        
        self.sprite.x = self.x
        self.sprite.y = self.y

class Player(Yukkuri):
    
    def __init__(self, world):        
        self.dfn = EntityDef.Find("Reimu")
        super(Player, self).__init__(world)
        self.up = 0
        self.down = 0
        self.left = 0
        self.right = 0
        self.action = "use"
        self.inventory = Inventory(self, 8, world.window)
        self.sprite.color = self.dfn.color        
        self.label = pyglet.text.Label(
            text="Player", group=self.sprite.group, batch=self.sprite.batch,
            x = self.x - 20, y = self.y - 20, color = (200, 30, 30, 255))
        self.label.font_size = 14
        self.wantparty = False

    
    def show_stats(self):
        self.inventory.stats_cahnge()
        if self.inventory.statssw:
            self.inventory.statssw = False
        else:
            self.inventory.statssw = True

    def use(self):        
        item = self.closer(120, self.world.items)
        if hasattr(item, "edible") and item.edible:
                    self.eat(item)
        elif hasattr(item, "takeable") and item.takeable:
                    self.take(item)

    def talk(self):
        ent = self.closer(140, self.world.ents)
        if ent:
            self.dialogue_start(ent)
        else:
            self.dialogue_start(dfn="selftalking")

    def move_camera(self): # rewrite in future        
        if not self.world.player:            
            return
        scale = 1        
        layer.gl.glMatrixMode(layer.gl.GL_PROJECTION)
        layer.gl.glLoadIdentity()
        layer.gl.gluOrtho2D(
            math.floor(-scale * self.world.window.width / 2.0),
            math.floor(+scale * self.world.window.width / 2.0),
            math.floor(-scale * self.world.window.height / 2.0),
            math.floor(+scale * self.world.window.height / 2.0))
    
        # Set modelview matrix to move, scale & rotate to camera position
        layer.gl.glMatrixMode(layer.gl.GL_MODELVIEW)
        layer.gl.glLoadIdentity()
        layer.gl.gluLookAt(
            math.floor(self.world.player.x),
            math.floor(self.world.player.y), +1.0,
            math.floor(self.world.player.x),
            math.floor(self.world.player.y), -1.0,
            0, 1, 0)
    
    def restore_camera(self):
        scale = 1        
        layer.gl.glMatrixMode(layer.gl.GL_PROJECTION)
        layer.gl.glLoadIdentity()
        layer.gl.gluOrtho2D(
            math.floor(-scale * self.world.window.width / 2.0),
            math.floor(+scale * self.world.window.width / 2.0),
            math.floor(-scale * self.world.window.height / 2.0),
            math.floor(+scale * self.world.window.height / 2.0))
    
        # Set modelview matrix to move, scale & rotate to camera position
        layer.gl.glMatrixMode(layer.gl.GL_MODELVIEW)
        layer.gl.glLoadIdentity()
        layer.gl.gluLookAt(
            math.floor(self.world.window.width / 2.0),
            math.floor(self.world.window.height / 2.0), +1.0,
            math.floor(self.world.window.width / 2.0),
            math.floor(self.world.window.height / 2.0), -1.0,
            0, 1, 0)

    def movie_floor(self):
        dx, dy = self.dx, self.dy
        if dx == dy: #WTF??? id(self.x) == id(self.y). It's not good
            if dx > 0: dx += 1
            else: dx -= 1
        for d in (dx, dy):            
            if d == dx:
                var = "x"
                player = self.x
            else:
                var = "y"
                player = self.y
            obj = self.world.floorsprites[var]
            keys = obj.keys()
            v1 = max(keys)
            v2 = min(keys)
            bs = - self.world.blocksize
            if d >= 0:
                v1, v2 = v2, v1
                bs = -bs                        
            if abs(player - v1) > 600:
                newx = v2 + bs
                if not obj.has_key(newx):
                           obj[newx] = []
                obj[newx] = obj[v1]
                for i in obj[v1]:
                    if var == "x":
                        i.x = newx
                    else:
                        i.y = newx
                del obj[v1]

    def cmove(self, action):
        if self.dialogue.started:
            self.dialogue.started.cmove(action)

    def choose(self):
        if self.dialogue.started:
            self.dialogue.started.make_choose(self.dialogue)

    def tick(self, dt):
        self.update(dt)
        if self.timerstatus and self.timer > self.tint:
            self.timerstatus = False
        left = self.left
        right = self.right
        up = self.up
        down = self.down        
        self.dx = right - left
        self.dy = up - down
        if self.dx != 0 or self.dy != 0:
            self.movie_floor()
            self.go_to(dt)            
            self.world.x += self.dx
            self.world.y += self.dy
            self.label.x += self.dx
            self.label.y += self.dy
            self.world.time.dark.x = self.world.time.light.x = self.world.x
            self.world.time.dark.y = self.world.time.light.y = self.world.y
            self.inventory.move(self.dx, self.dy)            
            self.move_camera()

class Bot(Yukkuri):
    rage = 1

    def __init__(self, world, x=None, y=None):        
        type = prob(**world.chances)        
        self.dfn = EntityDef.Find(type)
        super(Bot, self).__init__(world)
        if x: self.x = x
        if y: self.y = y        
        self.full = 0        
        self.exp = random.randint(0, self.world.player.level*self.world.player.level*1000)
        self.wantparty = False        
        if random.randint(0,100) >= 30:
            self.wantparty = True

    def run_away(self, x, y):
        posx, posy = 1,1
        if self.x < x:
            posx = -1
        if self.y < y:
            posy = -1
        self.force_go_to = (self.x+500*posx, self.y+500*posy)

    def find_party(self):
        if self.wantparty and not self.party:
            member = self.closer(700, self.world.ents, self.partyreject)
            if not member:
                return False
            if dist(self, member) < 50:
                if member == self.world.player:
                    if not member.party:
                        self.dialogue_start(member, dfn="playerjoin")
                    else:
                        self.party_reject(member)
                else:
                    self.dialogue_start(member, dfn="botjoin")
            else:
                self.force_go_to = (member.x + 40, member.y + 40)
            return True
        return False  

    def tick(self, dt):
        #need in rewrite
        self.update(dt)
        if self.dst_to_player() > 2000:
            self.kill()
            self.world.spawner.count -= 1          
        elif self.hp < 0:
            self.dead()
            self.world.spawner.count -= 1
        elif self.hungry and not self.force_go_to:
            item = self.closer(2000, self.world.items)
            if item:
                if dist(self, item) < 200 * self.sprite.scale:
                    self.eat(item)
                else:
                    self.force_go_to = (item.x+20, item.y-20)
            else:
                victim = self.closer(2000, self.world.ents)
                if victim:
                    self.attacked = victim
                else:
                    self.force_go_to = (self.x+300, self.y+200)
        elif self.timerstatus and self.timer > self.tint:
            self.timerstatus = False            
            if not self.hungry and not self.force_go_to:
                if not self.find_party():
                    if random.randint(0, 100) < 30:
                        e = (self.x + random.randint(-300,300), self.y + random.randint(-300,300))                     
                        if math.sqrt((self.x - e[0]) * (self.x - e[0]) + (self.y - e[1]) * (self.y - e[1])) > 100:
                            self.force_go_to = e            
        else:
            if self.force_go_to and not self.dialogue.started:
                left = self.force_go_to[0]  < self.x
                right = self.force_go_to[0]  > self.x
                down = self.force_go_to[1]  < self.y
                up = self.force_go_to[1]  > self.y
                dx = self.x - self.force_go_to[0]
                dy = self.y - self.force_go_to[1]
                pre_dist = dx * dx + dy * dy
                dx = right - left
                dy = up - down
                if abs(self.x - self.force_go_to[0]) < 2 and abs(self.y - self.force_go_to[1]) < 2:
                    self.force_go_to = None
            elif not self.eating:
                dx, dy = self.world.seek(self, confusion=200)
            self.dx = dx
            self.dy = dy        
            self.go_to(dt)

class Item(Entity):

    def __init__(self, dfn, world):
        super(Item, self).__init__()
        self.dfn = dfn
        self.world = world
        self.hp = self.dfn.hp
        self.nutritive = self.dfn.nutritive
        if hasattr(self.dfn, "down_anim") and len(self.dfn.down_anim) > 0:
            self.images = self.dfn.down_anim
            self.image = self.images[0]
        else:
             self.image = self.dfn.image
        self.sprite = Sprite(
            image=self.image, group=self.world.spritems, batch=world.sprites_batch)
        self.sprite.color = self.dfn.color
        self.days = 0
        self.world.items.add(self)
        if hasattr(self.dfn, "edible"):
            self.edible = self.dfn.edible
        elif hasattr(self.dfn, "takeable"):
            self.takeable = self.dfn.takeable

    def cross(self, other):
        for attr in ('hp_delta', 'rage_delta', 'satiation_delta'):
            v1 = getattr(self, attr)
            v2 = getattr(other, attr)
            v1, v2 = min(v1, v2), max(v1, v2)
            v1 -= 0.3
            v2 += 0.3
            v = random.uniform(v1, v2)
            setattr(self, attr, clamp(v, 0, 2))

    def tick(self, dt):
        if self.hp <= 0 or self.dst_to_player() > 2000:
            if self in self.world.items:
                self.world.items.remove(self)
                self.world.spawner.edibles -= 1
            self.kill()

    def draw(self):        
        if hasattr(self, "images"):
            imgidx = int(self.world.timer.get())
            max = len(self.images) - 1
            if imgidx > max: imgidx = max    
            img = self.images[imgidx]
            if img != self.sprite.image:
                self.sprite.image = img
        if self.sprite.position != (self.x, self.y):
            self.sprite.position = (self.x, self.y)

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
