# $Id: audio.py 424 2009-04-27 02:04:24Z joe.wreschnig $

import pyglet.media
import layer.load
import layer.command
import layer.task
import layer.const
import math
import weakref

from layer.parse import ParseObject
from layer.expr import Expression

__all__ = ["listener", "music", "AudioEvent", "AudioEventPlayer"]

# pyglet docs claim you can set the units for the listener, but it's a lie.
# So, we can set the units here instead.
class LayerListener(object):
    """Audio listener.

    Like pyglet's listener, but you can set a unit scale to apply to
    the positions of it, and AudioEventPlayers. It also only requires
    an (X, Y) coordinate and assumes Z is 0 if not given.

    All attributes other than scale and position are proxied to a
    pyglet listener.

    There is a single instance of this class, named listener, which
    you should use as your listener.
    """

    def __init__(self, listener=pyglet.media.listener, scale=None):
        self.__listener = listener
        self.scale = scale
        
    def _set_position(self, pos):
        if self.scale is None:
            try: scale = self.scale = layer.const.AUDIO_DISTANCE_SCALE
            except AttributeError: scale = 1.0
        try: x, y, z = pos
        except ValueError:
            x, y = pos
            z = 0
        self.__listener.position = (
            x * self.scale, y * self.scale, z * self.scale)

    def _get_position(self):
        if self.scale is None:
            try: scale = self.scale = layer.const.AUDIO_DISTANCE_SCALE
            except AttributeError: scale = 1.0
        x, y, z = self.__listener.position
        return x / self.scale, y / self.scale, z / self.scale

    position = property(_get_position, _set_position,
                        doc="Listener x/y/z position")

    def __getattr__(self, attr):
        return getattr(self.__listener, attr)
listener = LayerListener()
del(LayerListener)

class Music(object):
    """Play music tracks.

    'Music' is defined as audio that is looping, streaming, unaffected
    by listener location or orientation, and occupies the channel it
    is on exclusively.
    """
    def __init__(self):
        self.players = {}
        self.fading = {}
        self.task = layer.task.Task(self._update, enabled=True)
        self._volume = 1.0

    def _get_volume(self):
        return self._volume

    def _set_volume(self, volume):
        volume = max(0, min(2.0, volume))
        old_volume = self._volume
        if volume == old_volume:
            return
        for player in self.fading.keys() + self.players.values():
            player.volume = volume * player.volume / old_volume
        self._volume = volume

    volume = property(_get_volume, _set_volume, doc="Music volume")

    def play(self, filename, channel=0, fadein=2.0, fadeout=1.0):
        """Start playing music.

        A time to fade out the old music on the channel and fade in
        the new music can be provided.

        The channel value acts as a mutex; all other music players
        with the same channel are faded out when this file starts.
        """
        if not enabled:
            return
        self.stop(channel, fadeout)
        player = pyglet.media.Player()
        # Don't apply any directional/distance effects to music.
        player.min_distance = player.max_distance = 0
        player.eos_action = player.EOS_LOOP
        player.queue(layer.load.music(filename))
        player.volume = self.volume
        player.fadein = fadein

        # Hack to avoid skipping - loading a FLAC causes a noticeable
        # underrun on my system, and pyglet's defaults are around 0.2
        # seconds. Bump that to 0.5. Of course, thre's no actual
        # API for doing this...
        try: player._audio._update_buffer_time = 0.5 # OpenAL
        except AttributeError: pass
        try: player._audio._buffer_time = 0.5 # ALSA
        except AttributeError: pass
        player.play()
        self.players[channel] = player
        print_d("Playing music channel %d (%f sec)." % (channel, fadein))

    def _update(self, dt):
        vol = self.volume
        for player, fadetime in self.fading.items():
            dvolume = vol * dt / fadetime
            if dvolume >= player.volume:
                print_d("Stopping player.")
                player.pause()
                del(self.fading[player])
            else:
                player.volume -= dvolume
            if player.pitch != layer.task.Task.Scale:
                player.pitch = layer.task.Task.Scale
        for player in self.players.values():
            if player.volume < vol:
                player.volume = player.volume + dt * player.fadein * vol
            if player.pitch != layer.task.Task.Scale:
                player.pitch = layer.task.Task.Scale

    def stop(self, channel=0, fadetime=1.0):
        """Stop playing music."""
        if channel in self.players:
            print_d("Fading music channel %d (%f sec)" % (channel, fadetime))
            self.fading[self.players[channel]] = fadetime
            del(self.players[channel])

    def stop_all(self):
        for key in list(self.players.keys()):
            self.stop(key)

music = Music()
del(Music)

class AudioEvent(ParseObject):
    volume = Expression(1.0)
    pitch = Expression(1.0)
    follow = False

    load_spec = "*.audioevent"

    _shelves_ = ["volume", "pitch", "sound", "follow"]
    _freezers_ = dict(sound=lambda s: s.filename,
                      pitch="auto",
                      volume="auto",
                      )
    _thawers_ = dict(sound=layer.load.sound,
                     pitch="auto",
                     volume="auto",
                     )

    def play(self, cause=None):
        if not enabled: return None
        player = AudioEventPlayer(cause, self)
        player.play()
        return player

class AudioEventPlayer(pyglet.media.ManagedSoundPlayer):
    context = None
    task = None

    def __init__(self, cause, event):
        super(AudioEventPlayer, self).__init__()
        if not enabled: return
        self.cause = cause and weakref.ref(cause)
        self.event = event
        if self.cause:
            self.position = (cause.x * listener.scale,
                             cause.y * listener.scale,
                             getattr(cause, 'z', 0) * listener.scale)
        self.queue(event.sound)

    def play(self):
        if not enabled: return
        super(AudioEventPlayer, self).play()
        special = self.event.follow
        if self.event.volume.constant:
            self.volume = self.event.volume()
        else:
            special = True
        if self.event.pitch.constant:
            self.pitch = self.event.pitch() * layer.task.Task.Scale
        else:
            special = True
        self.clocktime = 0

        if special:
            self.context = layer.expr.EvalContext(
                "Audio Event %s" % self.event.name,
                dict(player=self, cause=self.cause(), event=self.event,
                     sound=self.event.sound, math=math))
            self.task = layer.task.Task(self._update, enabled=True)
            self._update(0)

    def stop(self):
        super(AudioEventPlayer, self).stop()
        if self.task:
            self.task.disable()
        self.context = None
        self.task = None
        self.cause = None
        self.sound = None

    def _update(self, dt):
        self.clocktime += dt
        cause = self.cause()
        if cause is not None and self.event.follow:
            self.position = (cause.x * listener.scale,
                             cause.y * listener.scale,
                             getattr(cause, 'z', 0) * listener.scale)

        if self.event.volume:
            self.volume = self.event.volume(context=self.context)
        if self.event.pitch:
            self.pitch = (self.event.pitch(context=self.context) *
                          layer.task.Task.Scale)

layer.command.command(music.play, name="music_play",
                      declspec=[str, int, float, float])
layer.command.command(music.stop, name="music_stop", declspec=[int, float])
layer.command.command(music.stop_all, name="music_stop_all")

layer.command.attr_command(music, "volume", name="music_volume",
                           doc="Set the music volume")

layer.command.attr_command(listener, "volume", name="volume",
                           doc="Set the global volume")

layer.command.attr_command(listener, "scale", name="audio_distance_scale",
                           doc="Set the global distance scale (in meters)",
                           declspec=[float])

enabled = True
@layer.command.command(declspec=[layer.command.cmd_bool])
def audio(val=None):
    """Enable/disable audio."""
    global enabled
    if val is None:
        return enabled
    enabled = val
    if not enabled:
        music.stop_all()

if pyglet.media.have_avbin:
    print_d("AVbin detected.")
else:
    print_d("AVbin not found.")
