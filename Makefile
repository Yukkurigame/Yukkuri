
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
3RDPARTYDIR= 3rdparty/


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -O0 -g -Wall $(DEFINES)


UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp Unit.cpp Animated.cpp Plant.cpp \
		 Corpse.cpp Dynamic.cpp Entity.cpp Player.cpp
GRAPHICS = Font.cpp Text.cpp ElasticBox.cpp sdl_graphics.cpp gl_extensions.cpp Animation.cpp \
		   Render.cpp pngfuncs.c
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp proto.cpp \
		  api/UnitManager.cpp api.cpp 
WIDGETS = Widget.cpp TextWidget.cpp BarWidget.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))
3RDPARTY = CUData.cpp CUDataUser.cpp CUDataTemplates.cpp


SRCS =   main.cpp yukkuri.cpp config.cpp engine.cpp Bindings.cpp BindFunctions.cpp Spawner.cpp \
		 map.cpp \
         $(addprefix $(SCRIPTSDIR), $(SCRIPTS)) \
         $(addprefix $(UNITSDIR), $(UNITS)) \
         $(addprefix $(GRAPHICSDIR), $(GRAPHICS)) \
         $(addprefix $(INTERFACEDIR), $(INTERFACE)) \
         $(addprefix $(3RDPARTYDIR), $(3RDPARTY)) \
         Camera.cpp daytime.cpp

OBJ = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJDIR), $(OBJ:.c=.o))


UNIQHEADERS = $(GRAPHICSDIR)GraphicsTypes.h $(SCRIPTSDIR)LuaScriptConfig.h \
         $(3RDPARTYDIR)TypeList.h Define.h debug.h hacks.h safestring.h types.h 

HEADERS = $(OBJS:.o=.h) $(addprefix $(OBJDIR), $(UNIQHEADERS))
 
 
GCHOLD = $(HEADERS:.h=.h.gch)
GCH = $(shell echo $(GCHOLD) | sed -e "s/[^ ]\+\/\(main\|LuaRegister\).h.gch //g")


#.cpp.o:
#FIXME: only gnu make?
$(OBJDIR)%.h.gch: %.h
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OBJDIR)%.o: %.cpp
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)%.o: %.c
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@


#$*.cpp

.PHONY: all clean

all: $(PROGNAME)

$(PROGNAME) : | $(OBJDIR) $(GCH) $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

$(OBJDIR):
	mkdir -p $(addprefix $(OBJDIR), $(SCRIPTSDIR) $(SCRIPTSDIR)api $(UNITSDIR) \
	 $(GRAPHICSDIR) $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR) $(3RDPARTYDIR))

clean: cleanheaders cleanobjs cleandirs

cleanheaders:
	$(rm) $(GCH)

cleanobjs:
	$(rm) $(OBJS) $(PROGNAME)
	
cleandirs:
	$(rm) -r $(OBJDIR)
