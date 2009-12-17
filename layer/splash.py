# $Id: splash.py 264 2009-03-16 06:24:23Z piman $

from layer.command import command
from layer.parse import ParseObject
from layer.sprite import Sprite
from pyglet import clock, app

class Splash(ParseObject):
    """A pointless splash screen."""
    def __init__(self, window, filename='splash.png'):
        self.sprite = Sprite(filename=filename)
        self.sprite.x = window.width // 2 - self.sprite.width // 2
        self.sprite.y = window.height // 2 - self.sprite.height // 2
        self.window = window

        clock.schedule_once(self.remove, 4.0)
        self.window.push_handlers(on_draw=self.draw)

    def draw(self):
        self.sprite.draw()

    def remove(self, dt):
        self.window.remove_handlers(on_draw=self.draw)

@command
def splash():
    """Show the splash screen."""
    for window in app.windows:
        Splash(window, filename='splash.png')
        break
