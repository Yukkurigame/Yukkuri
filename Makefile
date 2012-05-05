
rm=/bin/rm -f
CC= g++
#VPATH= 
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iscripts -Iunits -Igraphics -Iinterface -I/usr/include/freetype2
LIBS= $(shell sdl-config --libs) -lpng -lSDL_image -lGL -lfreetype -llua
OBJDIR= obj/
SCRIPTSDIR= scripts/
UNITSDIR= units/
GRAPHICSDIR= graphics/
WIDGETSDIR= widgets/
INTERFACEDIR= interface/


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -O0 -g -pg -Wall $(DEFINES)


UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp Unit.cpp Animated.cpp Plant.cpp \
		 Corpse.cpp Dynamic.cpp Entity.cpp Player.cpp
GRAPHICS = Font.cpp Text.cpp ElasticBox.cpp sdl_graphics.cpp gl_extensions.cpp Animation.cpp \
		   Render.cpp pngfuncs.c
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp api.cpp proto.cpp
WIDGETS = Widget.cpp TextWidget.cpp BarWidget.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))


SRCS =   main.cpp yukkuri.cpp config.cpp engine.cpp Bindings.cpp BindFunctions.cpp Spawner.cpp \
		 map.cpp \
         $(addprefix $(SCRIPTSDIR), $(SCRIPTS)) \
         $(addprefix $(UNITSDIR), $(UNITS)) \
         $(addprefix $(GRAPHICSDIR), $(GRAPHICS)) \
         $(addprefix $(INTERFACEDIR), $(INTERFACE)) \
         Camera.cpp daytime.cpp

OBJ = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJDIR), $(OBJ:.c=.o))

#.cpp.o:
#FIXME: only gnu make?
$(OBJDIR)%.o: %.cpp
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)%.o: %.c
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@


#$*.cpp

.PHONY: all clean

all: $(PROGNAME)

$(PROGNAME) : | $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

$(OBJDIR):
	mkdir -p $(addprefix $(OBJDIR), $(SCRIPTSDIR) $(UNITSDIR) $(GRAPHICSDIR) \
			 $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR))

clean: cleanobjs cleandirs

cleanobjs:
	$(rm) $(OBJS) $(PROGNAME)
	
cleandirs:
	$(rm) -r $(OBJDIR)
