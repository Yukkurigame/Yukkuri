
rm=/bin/rm -f
CC= g++
VPATH = ./units ./graphics
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iunits -Igraphics
LIBS= $(shell sdl-config --libs) -lSDL_image -lGL -lfreetype -Llibs  -llua


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -Wall $(DEFINES)



UNITS =  unitmanager.cpp unit.cpp Dynamic.cpp Entity.cpp Player.cpp
GRAPHICS = Font.cpp Graphics.cpp Interface.cpp Widgets.cpp

SRCS =   main.cpp engine.cpp Lua.cpp Luaconfig.cpp Bindings.cpp\
         $(UNITS) \
         $(GRAPHICS) \
         Camera.cpp daytime.cpp

OBJS = $(SRCS:.cpp=.o)

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