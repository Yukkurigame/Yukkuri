
rm=/bin/rm -f
CC= g++
#VPATH= 
DEFS= $(shell sdl-config --cflags)
PROGNAME= Yukkuri
INCLUDES=  -I. -Iscripts -Iunits -Igraphics -Iinterface -I/usr/include/freetype2
LIBS= $(shell sdl-config --libs) -lpng -lSDL_image -lGL -lfreetype -llua
OBJDIR= obj/
SCRIPTSAPIDIR = api/
SCRIPTSDIR= scripts/
UNITSDIR= units/
GRAPHICSDIR= graphics/
WIDGETSDIR= widgets/
INTERFACEDIR= interface/
MAPDIR = map/
3RDPARTYDIR= 3rdparty/


DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1 -DGL_GLEXT_PROTOTYPES=1
CFLAGS= -O0 -g -Wall $(DEFINES)


UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp Unit.cpp Animated.cpp Plant.cpp \
		 Corpse.cpp Dynamic.cpp Entity.cpp Player.cpp
RENDER = Atlas.cpp ElasticBox.cpp GLHelpers.cpp TextureArray.cpp
GRAPHICS = GraphicsTypes.cpp Font.cpp Text.cpp sdl_graphics.cpp gl_extensions.cpp gl_shader.cpp \
		   Animation.cpp Camera.cpp Render.cpp pngfuncs.c $(addprefix render/, $(RENDER))
SCRIPTSAPI = UnitManager.cpp Interface.cpp Widgets.cpp ThreadManager.cpp CameraApi.cpp
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp proto.cpp \
		  api.cpp $(addprefix $(SCRIPTSAPIDIR), $(SCRIPTSAPI))
WIDGETS = Widget.cpp WidgetText.cpp WidgetBar.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))
MAP = Tiles.cpp Region.cpp Map.cpp
3RDPARTY = CUData.cpp CUDataUser.cpp CUDataTemplates.cpp LuaPusher.cpp timer/TimerManager.cpp


SRCS =   main.cpp yukkuri.cpp config.cpp engine.cpp Bindings.cpp BindFunctions.cpp \
         $(addprefix $(SCRIPTSDIR), $(SCRIPTS)) \
         $(addprefix $(UNITSDIR), $(UNITS)) \
         $(addprefix $(GRAPHICSDIR), $(GRAPHICS)) \
         $(addprefix $(INTERFACEDIR), $(INTERFACE)) \
         $(addprefix $(MAPDIR), $(MAP)) \
         $(addprefix $(3RDPARTYDIR), $(3RDPARTY)) \
         daytime.cpp

OBJ = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJDIR), $(OBJ:.c=.o))


UNIQHEADERS = $(SCRIPTSDIR)LuaScriptConfig.h \
         	$(addprefix $(3RDPARTYDIR), TypeList.h timer/InternalTimerEvent.h \
         	timer/InternalTimerEvent.h timer/TimerEvent.h timer/ITimerEventPerformer.h ) \
         	Define.h debug.h hacks.h safestring.h types.h 

HEADERS = $(OBJS:.o=.h) $(addprefix $(OBJDIR), $(UNIQHEADERS))
 
 
GCHOLD = $(HEADERS:.h=.h.gch)
GCH = $(shell echo $(GCHOLD) | sed -e "s/[^ ]\+\/\(main\|LuaRegister\|api\/Widgets\).h.gch //g")


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

rebuild: cleanprog all

$(PROGNAME) : | $(OBJDIR) $(GCH) $(OBJS)
	$(CC) $(CFLAGS)  -o $(PROGNAME) $(OBJS) $(LIBS)

$(OBJDIR):
	mkdir -p $(addprefix $(OBJDIR), $(SCRIPTSDIR) $(SCRIPTSDIR)$(SCRIPTSAPIDIR) $(UNITSDIR) \
	 $(GRAPHICSDIR) $(GRAPHICSDIR)render/ $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR) \
	 $(MAPDIR) $(3RDPARTYDIR) $(3RDPARTYDIR)timer)

clean: cleanheaders cleanobjs cleanprog cleandirs

cleanheaders:
	$(rm) $(GCH)

cleanobjs:
	$(rm) $(OBJS)

cleanprog:
	$(rm) $(PROGNAME)

cleandirs:
	$(rm) -r $(OBJDIR)
