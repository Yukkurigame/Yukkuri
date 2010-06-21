
rm=/bin/rm -f
CC= g++
VPATH = ./units
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iunits
LIBS= $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -Llibs  -llua


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -Wall $(DEFINES)



UNITS =  unitmanager.cpp unit.cpp Dynamic.cpp Entity.cpp Player.cpp

SRCS =   main.cpp yloader.cpp engine.cpp Luaconfig.cpp \
         $(UNITS) \
         Camera.cpp Graphics.cpp \
         daytime.cpp
         # ConfigManager.cpp ConfigTypes.cpp \

OBJS = $(SRCS:.cpp=.o)

.cpp.o:
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@

#$*.cpp

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)


clean:
	$(rm) $(OBJS) $(PROGNAME)