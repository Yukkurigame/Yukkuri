# $Id: tiles.py 262 2009-03-16 06:20:38Z piman $

from layer.parse import ParseObject, dump
from layer.command import command
from layer.sprite import Sprite
from pyglet.graphics import Batch, OrderedGroup
from pyglet import app

__all__ = ['TileDef', 'Tile', 'TileLayer', 'TileMapDef', 'TileMap']

get_definition = lambda cls, definition_name: cls.definitions[definition_name]

class TileDef(ParseObject):
    definitions = {}
    get = classmethod(get_definition)

    def __init__(self, name='Unnamed Tile', image='', flags=['blocking']):
        self.name = name
        self.flags = flags
        self.image = image
        TileDef.definitions[name] = self

class Tile(ParseObject):
    def __init__(self, definition='grass', batch=None, group=None):
        self.definition = TileDef.get(definition)
        self.__dict__.update(self.definition.__dict__)

        self._sprite = Sprite(filename=self.definition.image,
                                batch=batch, group=group)
        self._contents = []

    def _set_contents(self, contents):
        self._contents = contents
    def _get_contents(self):
        return self._contents
    contents = property(_get_contents, _set_contents)

    def _set_sprite(self, sprite):
        self._sprite = sprite
    def _get_sprite(self):
        return self._sprite
    sprite = property(_get_sprite, _set_sprite)

    def __getstate__(self):
        return {
            'definition': self.definition.name,
            'contents': self.contents
        }

    def __setstate__(self, state):
        self.__init__(definition=state['definition'])
        self.contents = state['contents']

class TileLayer(ParseObject):
    def __init__(self, name='Layer', batch=None, z_index=0, hidden=False, tiles=[]):
        self.name = name
        self.z_index = z_index
        self.group = OrderedGroup(self.z_index)
        self.hidden = hidden
        self.batch = batch
        self.tiles = []

        for tile in tiles:
            self.tiles += [Tile(definition=tile, batch=self.batch,
                                group=self.group)]
        if self.hidden:
            self.hide()

    def hide(self):
        for tile in self.tiles:
            tile.sprite.batch = None

    def show(self):
        for tile in self.tiles:
            tile.sprite.batch = self.batch

class TileMapDef(ParseObject):
    definitions = {}
    get = classmethod(get_definition)

    def __init__(self, name='Unnamed Map', size=(20, 15),
                tile_size=(32, 32), background=None, layers={}):
        self.name = name
        self.size = size
        self.tile_size = tile_size
        self.background = background
        self.layers = layers
        TileMapDef.definitions[name] = self

class TileMap(ParseObject):
    def __init__(self, definition='', batch=None, origin=(0, 0)):
        self.definition = TileMapDef.get(definition)
        self.__dict__.update(self.definition.__dict__)

        self.origin = origin

        self.layers = {}
        for name, layer in self.definition.layers.iteritems():
            self.layers[name] = TileLayer(name=name, batch=batch, **layer)

        self.update_layout()

    def tile_at(self, layer, x, y):
        return self.layers[layer].tiles[x + y * self.size[1]]

    def update_layout(self):
        # update sprite positions for the tiles according to layout
        for layer in self.layers.itervalues():
            row, col = 0, 0
            # reverse tile rows because of GL's coords
            reordered_tiles = []
            for r in range(self.size[1] - 1, -1, -1):
                reordered_tiles += layer.tiles[r * self.size[1]: r * self.size[1] + self.size[1]]

            # set tile positions accordingly
            for tile in reordered_tiles:
                if col >= self.size[1]:
                    row += 1
                    col = 0
                tile.sprite.x = self.origin[0] + col * self.tile_size[0]
                tile.sprite.y = self.origin[1] + row * self.tile_size[1]
                col += 1

class TileEngine(ParseObject):
    def __init__(self, window, origin=(0, 0)):
        self.window = window
        self.origin = origin
        self.batch = Batch()
        self.map = None

        TileDef(name='grass', image='grass.png', flags=[])
        TileDef(name='wall', image='wall.png')
        TileDef(name='debug', image='debug.png')
        TileDef(name='transparent', image='transparent.png')
        TileMapDef(
            name='test_01',
            size=(3,3),
            layers={
                'ground': {
                    'z_index': 0,
                    'hidden': False,
                    'tiles': [
                        'grass',    'wall',     'wall',
                        'wall',     'grass',    'wall',
                        'grass',    'grass',    'wall'
                    ]
                },
                'layer2': {
                    'z_index': 1,
                    'hidden': True,
                    'tiles': [
                        'debug',    'debug',    'debug',
                        'debug',    'debug',    'debug',
                        'debug',    'debug',    'debug'
                    ]
                }
            }
        )

        window.push_handlers(on_draw=self.on_draw)
        print_d('TileEngine created.')

    def load_map(self, map_name):
        self.map = TileMap(definition=map_name, batch=self.batch, origin=self.origin)

    def on_draw(self):
        self.batch.draw()

@command
def tile_test():
    """Test the tile engine."""
    for window in app.windows:
        TileEngine(window, origin=(200, 200)).load_map('test_01')
        break
