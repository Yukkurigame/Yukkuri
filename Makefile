
rm=/bin/rm -f
CC= g++
VPATH = ./scripts ./units ./graphics
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iscripts -Iunits -Igraphics -I/usr/include/freetype2
LIBS= $(shell sdl-config --libs) -lpng -lSDL_image -lGL -lfreetype -llua


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -Wall $(DEFINES)


UNITS =  unitmanager.cpp unit.cpp Animated.cpp Plant.cpp Corpse.cpp Dynamic.cpp Entity.cpp Player.cpp
GRAPHICS = Font.cpp Graphics.cpp Interface.cpp Widgets.cpp pngfuncs.c
SCRIPTS = Lua.cpp Luaconfig.cpp LuaScript.cpp LuaThread.cpp api.cpp

SRCS =   main.cpp config.cpp engine.cpp Bindings.cpp Spawner.cpp map.cpp\
         $(SCRIPTS) \
         $(UNITS) \
         $(GRAPHICS) \
         Camera.cpp daytime.cpp

OBJ = $(SRCS:.cpp=.o)
OBJS = $(OBJ:.c=.o)

.cpp.o:
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@


#$*.cpp

.PHONY: all clean

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	$(rm) $(OBJS) $(PROGNAME)
