import pyglet

import layer.colors
import layer.sprite
import layer.interp

from layer.parse import ParseObject

class Label(pyglet.text.Label):
    def __init__(self, *args, **kwargs):
        batch = kwargs.setdefault("batch", layer.sprite.batch)
        z = kwargs.pop("z", None)
        if z is not None:
            try: kwargs.setdefault("group", batch.Group(z))
            except AttributeError: pass
        super(Label, self).__init__(*args, **kwargs)

def px_to_pt(px, dpi=96):
    return px and ((72 * px) / dpi)
        
class Font(ParseObject, layer.interp.Interpolatable):
    load_spec = "*.font"

    _shelves_ = [
        "name", "face", "color", "px", "pt", "bold", "italic", "opacity"]
    name = None
    face = "Layer Monospace"
    px = None
    pt = None
    bold = False
    italic = False
    color = (255, 255, 255)
    opacity = 1.0

    _freezers_ = dict(color=layer.colors.name)
    _thawers_ = dict(color=layer.colors.value)
    _interpolate_ = ["color", "px", "pt", "opacity"]

    def Label(self, text, px=None, pt=None, **kwargs):
        opacity = int(kwargs.pop("opacity", 255) * self.opacity)
        kwargs.setdefault("color", self.color + (opacity,))
        kwargs.setdefault("font_name", self.face)
        kwargs.setdefault("bold", self.bold)
        kwargs.setdefault("italic", self.italic)
        kwargs.setdefault(
            "font_size",
            pt or px_to_pt(px) or self.pt or px_to_pt(self.px) or 12)
        
        label = Label(text, **kwargs)
        label.font = self
        return label
