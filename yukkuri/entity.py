
from layer.sprite import Sprite, Group, OrderedGroup, batch, Batch
from misc import *

class Entity(Entity):
    dfn = None
    force_go_to = None

    @property
    def block(self):
        return (int(self.x / layer.const.BLOCK_SIZE),
                int(self.y / layer.const.BLOCK_SIZE))

    def dst_to_player(self):
         return dist(self, self.world.player)

class EntityDef(layer.parse.ParseObject):
    load_spec = "*.item"
    _thawers_ = dict(color=layer.colors.value)
    _freezers_ = dict(color=layer.colors.name)
    image = None
    down_anim = []
    up_anim = []
    left_anim = []
    right_anim = []
    anchor_x = None
    anchor_y = None
    takeable = False
    speed = 100
    anim_distance = 10
    hp = 0
    damage = 0
    color = (255, 255, 255)
    growth = []
    days_to_grow = 1    
    type = None

    def __getstate__(self):
        d = super(EntityDef, self).__getstate__()
        try: d["image"] = d["image"].filename
        except KeyError: pass
        except AttributeError: del(d["image"])

        for attr in ["down_anim", "up_anim", "left_anim", "right_anim",
                     "growth"]:
            try: d[attr] = map(lambda i: i.filename, d[attr])
            except KeyError: pass
            except AttributeError: del(d[attr])
        return d
    
    def __setstate__(self, d):
        super(EntityDef, self).__setstate__(d)
        if self.image:
            self.image = layer.load.image(self.image)
            self.image.anchor_x = self.anchor_x or self.image.width / 2
            self.image.anchor_y = self.anchor_y or self.image.height / 2
        for attr in ["down_anim", "up_anim", "left_anim", "right_anim",
                     "growth"]:
            l = getattr(self, attr)
            if l:
                l = map(layer.load.image, l)
                setattr(self, attr, l)
                for image in l:
                    image.anchor_x = self.anchor_x or image.width / 2
                    image.anchor_y = self.anchor_y or image.height / 2

class Yukkuri(Entity):    

    def __init__(self, world):
        super(Yukkuri, self).__init__()        
        self.world = world        
        self.x = world.width / 2
        self.y = world.height / 2
        self.images = self.dfn.down_anim
        self.sprite = Sprite(image=self.images[0], group=world.sprites,
                             batch=world.sprites_batch)
        self.imgmini = self.dfn.mini
        self.sprite.color = (255, 255, 255)
        self.type = self.dfn.name
        self.sprite.scale = 0.3
        self.bloodcolor = eval(self.dfn.bloodcolor)        
        self.dialogue = Dialogue(self)
        self.attacked = None
        self.eating = None
        self.hungry = False
        self.predator = False
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
        self.party = None
        self.rapespeed = 0.1
        if hasattr(self.dfn, "rapespeed"): self.rapespeed = self.dfn.rapespeed
        self.rapebar = 0
        self.blocked = False
        self.grows_into = [self.type]
        self.raping, self.raped, self.growth, self.days_to_grow = None, None, None, None 
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
            speed = abs(self.dfn.speed * self.fed * 2) #(self.dfn.speed * self.world.map.speed(self.x, self.y))            
            if speed < 50:
                speed = 50
            self.dx = dt * speed * self.dx / l
            self.dy = dt * speed * self.dy / l
            nx = self.x + self.dx
            ny = self.y + self.dy
            nx, ny = self.world.valid_pos(self.x, self.y, nx, ny)
            #if self.x - 1 <= nx <= self.x + 1 and self.y - 1 <= ny <= self.y + 1:
                #if self.dx == 0: self.dx = random.uniform(-0.2, 0.2)
                #if self.dy == 0: self.dy = random.uniform(-0.2, 0.2)
                #self.round_go = (self.x-self.dx*random.randint(4,24), 
                                          #self.y-self.dy*random.randint(4,24))
                #self.dx, self.dy = 0, 0
            #else: 
            distance = math.sqrt((nx - self.x) * (nx - self.x) + (ny - self.y) * (ny - self.y))
            self.distance += distance                 
            self.x =  nx
            self.y = ny
            if self.fed >= 0.01:
                self.fed -= distance*0.0001/(self.stats["Agi"]+math.sqrt(self.stats["Sta"]))

    def attack(self, victim = None):
        if self.blocked:
            return
        if not victim:
            if self.attacked and dist(self, self.attacked)*self.sprite.scale <= 120:
                victim = self.attacked
            else: 
                victim = self.closer(120, self.world.ents)
        if victim and dist(self, victim) < 120:
            self.dialogue_start(dfn="selftalking", frame=2)
            if self.fed >= 0.01:
                self.fed -= 0.001/(self.stats["Agi"]+math.sqrt(self.stats["Sta"]))
            victim.hit(self)

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
        if self.party:            
            self.party.leave(self, killer)
        if self.dialogue.started:
            self.dialogue.leave()        
        self.kill()

    def rape(self, victim = None):
        if self.blocked:
            return
        if self.rapebar < 100:
            return
        if not victim:
            victim = self.closer(120, self.world.ents)
        if victim and dist(self, victim) < 120:
            self.rape_attempt(victim)

    def rape_attempt(self, victim):
        self.chanse = 1
        if self.party and victim not in self.party:
            if victim.level >= self.level:
                self.chanse = random.randint(self.level, victim.level) - self.level + 1
            if self.chanse > 2:
                victim.attacked = self
                return
        self.x = victim.x
        self.y = victim.y + victim.height/2
        self.dialogue_start(dfn="selftalking", frame=4)
        self.blocked = True
        victim.blocked = True                
        victim.raped = self        
        self.raping = victim

    def rape_end(self, victim):
        self.blocked = False
        victim.blocked = False
        self.x += 20
        if self.party and victim not in self.party:
            attack = random.randint(0, 100)
            if attack <= 5:
                victim.attacked = self
            elif victim.party:
                victim.party.enemy = self
        elif not self.party and not victim.party:
            join = random.randint(0, 100)
            if join >= 20:
                victim.join_party(self)
        if random.randint(0, 100) > 5:
            victim.growth = True
            victim.days_to_grow = self.dfn.days_to_grow
            victim.grows_into = [self.type, victim.type]
        self.raping = None
        victim.raped = None

    def born(self):
        for i in range(random.randint(3, 8)):
            child = Bot(self.world, 
                             self.x + random.randint(-30, 30), 
                             self.y + random.randint(-30, 30),
                             random.choice(self.grows_into), 1)
            self.world.ents.add(child)
            self.world.spawner.count += 1
            child.hp = random.uniform(-10, 10)
            child.join_party(self)
        self.growth = False
        self.grows_into = [self]
        self.force_go_to = (self.x + 20, self.y + 20)

    def eat(self, obj):
        if self.blocked:
            return
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
        if self.blocked:
            return
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
        if self.blocked:
            return
        if not self.dialogue.start(*obj):
            return
        dfn, first = "main", 0        
        if kwargs.has_key("dfn"): dfn = kwargs["dfn"] 
        if kwargs.has_key("frame"): first = kwargs["frame"]
        self.dialogue.started.set_dfn(dfn, first)

    def dialogue_update(self, dt):        
        ds = self.dialogue.started
        if not ds  or not ds.timerstatus:
            return
        if not self.blocked:
            self.blocked = True
        ds.timer += dt
        if ds.timer > ds.timerstatus:
            ds.next_frame()

    def closer(self, dst, objects, reject=[], **kwargs):
        def it(obj):
            if self is not obj:
                if not len(reject) or obj not in reject:
                    if kwargs.has_key("type") and obj.type is not kwargs["type"]:
                        return 
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
        if self.attacked and self.attacked.hp <= 0:
            self.attacked = None
        if self.exp >= self.max_exp:
            self.levelup()
        if self.fed < 0.7:
            if not self.hungry:
                self.dialogue_start(dfn="selftalk", frame=1)
            self.hungry = True
            if self.fed < 0.1:
                self.fed = 0.1
        if self.fed > 0.95:
            self.hungry = False
        if self.growth and self.days_to_grow < 0:
            self.born()
        if self.timer < 1:
            if not self.timerstatus: self.timerstatus = True
        if self.timerstatus and self.timer > self.tint:                        
            if self.raping:
                if self.raping.growth:
                    self.raping.growth = None
                if self.rapebar <= 0:
                    self.rape_end(self.raping)
                else:
                    self.dialogue.leave()
                    self.raping.dialogue.leave()
                    self.blocked = True
                    self.raping.blocked = True
                    self.rapebar -= self.level*10
                    self.fed -= 0.05
                    self.hp -= 0.1/self.level
                    self.raping.fed -= 0.05 + 0.1/self.raping.fed
                    self.raping.hp -= 0.01/self.raping.level
            else:                
                if self.rapebar < 100:
                    self.rapebar += self.rapespeed
            if self.party and not self.blocked:
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
        
        if self.growth:            
            self.images = self.dfn.growth 
            img = self.images[self.days_to_grow]
        
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
        self.wantparty = False

    
    def show_stats(self):
        self.inventory.stats_cahnge()
        if self.inventory.statssw:
            self.inventory.statssw = False
        else:
            self.inventory.statssw = True

    def use(self):        
        if self.blocked:
            return
        item = self.closer(120, self.world.items)
        if hasattr(item, "edible") and item.edible:
                    self.eat(item)
        elif hasattr(item, "takeable") and item.takeable:
                    self.take(item)

    def talk(self):
        if self.blocked:
            return
        ent = self.closer(140, self.world.ents)
        if ent:
            self.dialogue_start(ent)
        else:
            self.dialogue_start(dfn="selftalking")

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
        if self.rapebar >= 100:
            self.inventory.rapeshow()
        else:
            self.inventory.rapehide()
        self.inventory.party()
        if self.timerstatus and self.timer > self.tint:            
            self.timerstatus = False
        if self.blocked:
            return
        left = self.left
        right = self.right
        up = self.up
        down = self.down        
        self.dx = right - left
        self.dy = up - down        
        if self.dx != 0 or self.dy != 0:
            self.movie_floor()
            self.go_to(dt)

class Bot(Yukkuri):
    rage = 1

    def __init__(self, world, x=None, y=None, type=None, exp=None):
        if not type:        
            type = prob(**world.chances)
        self.dfn = EntityDef.Find(type)
        super(Bot, self).__init__(world)
        if x: self.x = x
        if y: self.y = y        
        self.full = 0
        if hasattr(self.dfn, "predator"):
            self.predator = self.dfn.predator
        self.exp = (exp or random.randint(0, self.world.player.level*self.world.player.level*1000))
        self.wantparty = False
        self.round_go = None 
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
                    if not member.party or member.wantparty:
                        self.dialogue_start(member, dfn="playerjoin")
                    else:
                        self.party_reject(member)
                else:
                    self.dialogue_start(member, dfn="botjoin")
            else:
                self.force_go_to = (member.x + 40, member.y + 40)
            return True
        return False  

    def go_offset(self, x, y):
        left = x  < self.x + 1
        right = x  > self.x - 1
        down = y  < self.y + 1
        up = y  > self.y - 1
        dx = self.x - x
        dy = self.y - y
        dx = right - left
        dy = up - down
        return (dx, dy)
        

    def tick(self, dt):
        #need in rewrite
        self.update(dt)
        if self.dst_to_player() > 2000:
            self.kill()
            self.world.spawner.count -= 1          
        elif self.hp < 0:
            self.dead()
            self.world.spawner.count -= 1
        elif self.hungry and not self.force_go_to and not self.blocked:
            if self.predator:
                item = self.closer(2000, self.world.items, type="meat")
            else:
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
            if not self.hungry and not self.force_go_to and not self.blocked:
                if not self.find_party():
                    if random.randint(0, 100) < 30:
                        e = (self.x + random.randint(-300,300), self.y + random.randint(-300,300))                     
                        if math.sqrt((self.x - e[0]) * (self.x - e[0]) + (self.y - e[1]) * (self.y - e[1])) > 100:
                            self.force_go_to = e            
        elif not self.blocked:
            dx, dy = None, None
            for c in (self.round_go, self.force_go_to):
                if c:
                    dx, dy = self.go_offset(*c)
                    if abs(self.x - c[0]) < 2 and abs(self.y - c[1]) < 2:
                        if c is self.round_go: self.round_go = None
                        if c is self.force_go_to: self.force_go_to = None
                        dx, dy = 0, 0
                    if c is self.round_go: break
            if not dx and not dy:
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
        self.type = self.dfn.type        
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
