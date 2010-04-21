##----------------------------------------------------------------------------
# Created with genmake.pl v1.1 on Thu Jul 30 00:33:59 2009
# genmake.pl home: http://muquit.com/muquit/software/
# Copryright: GNU GPL (http://www.gnu.org/copyleft/gpl.html)
##----------------------------------------------------------------------------
rm=/bin/rm -f
CC= g++
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I.
LIBS= $(shell sdl-config --libs) -lSDL_image -lSDL_ttf


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -Wall $(DEFINES)

SRCS =  main.cpp unit.cpp unitmanager.cpp yloader.cpp engine.cpp

OBJS =  main.o unit.o unitmanager.o yloader.o engine.o

.cpp.o:
	$(rm) $@
	$(CC) $(CFLAGS) -c $*.cpp

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	$(rm) $(OBJS) $(PROGNAME) core *~
