
rm=/bin/rm -f
CC= g++
VPATH = ./units
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iyaml/include -Iunits
LIBS= $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -Llibs/ -lyaml


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -Wall $(DEFINES)



UNITS =  unitmanager.cpp unit.cpp Dynamic.cpp Entity.cpp Player.cpp

SRCS =   main.cpp yloader.cpp engine.cpp \
         $(UNITS) \
         Camera.cpp Graphics.cpp ConfigManager.cpp ConfigTypes.cpp \
         daytime.cpp

#OBJS =  main.o yloader.o engine.o \
#        Camera.o Graphics.o ConfigManager.o ConfigTypes.o \
#        daytime.o
OBJS = $(SRCS:.cpp=.o)

.cpp.o:
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@

#$*.cpp

all: yaml $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

yaml:
	cd yaml/src && $(MAKE)

clean:
	$(rm) $(OBJS) $(PROGNAME)
	cd yaml/src && $(MAKE) clean