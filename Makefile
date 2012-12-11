
include Makefile.in


OBJDIR= obj/
OUTDIR= bin/
VPATH= src/
PROGNAME= Yukkuri
COREDIR= core/
SCRIPTSAPIDIR= api/
SCRIPTSDIR= scripts/
UNITSDIR= units/
PHYSICSDIR= $(UNITSDIR)physics/
GRAPHICSDIR= graphics/
WIDGETSDIR= widgets/
INTERFACEDIR= interface/
MAPDIR = map/
3RDPARTYDIR= 3rdparty/


CORE = yukkuri.cpp
UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp ActionTimer.cpp Unit.cpp \
		 UnitStatic.cpp UnitDynamic.cpp UnitEntity.cpp UnitCorpse.cpp UnitPlayer.cpp
PHYSICS = physics.cpp handlers.cpp
RENDER = Atlas.cpp ElasticBox.cpp GLHelpers.cpp GLTextures.cpp TextureArray.cpp
SPRITE = Sprite.cpp Animation.cpp AnimationDefines.cpp Brush.cpp
GRAPHUTILS = gl_shader.cpp pngfuncs.c sdl_graphics.cpp VBOArray.cpp
GRAPHICS = Camera.cpp daytime.cpp Font.cpp gl_extensions.cpp Render.cpp Text.cpp \
		   $(addprefix render/, $(RENDER)) $(addprefix sprite/, $(SPRITE)) $(addprefix utils/, $(GRAPHUTILS))
SCRIPTSAPI = UnitManagerApi.cpp InterfaceApi.cpp Widgets.cpp ThreadManagerApi.cpp CameraApi.cpp \
			 BindingsApi.cpp Units.cpp PathsApi.cpp
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp proto.cpp \
		  api.cpp $(addprefix $(SCRIPTSAPIDIR), $(SCRIPTSAPI))
WIDGETS = Widget.cpp WidgetText.cpp WidgetBar.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))
MAP = Tiles.cpp Region.cpp Map.cpp
3RDPARTY = CUData.cpp CUDataUser.cpp CUDataTemplates.cpp LuaPusher.cpp timer/TimerManager.cpp


SRCS =   main.cpp config.cpp misc.cpp Bindings.cpp BindFunctions.cpp \
         $(addprefix $(COREDIR), $(CORE)) \
         $(addprefix $(SCRIPTSDIR), $(SCRIPTS)) \
         $(addprefix $(PHYSICSDIR), $(PHYSICS)) \
         $(addprefix $(UNITSDIR), $(UNITS)) \
         $(addprefix $(GRAPHICSDIR), $(GRAPHICS)) \
         $(addprefix $(INTERFACEDIR), $(INTERFACE)) \
         $(addprefix $(MAPDIR), $(MAP)) \
         $(addprefix $(3RDPARTYDIR), $(3RDPARTY)) \
         utf.cpp

OBJ = $(SRCS:.cpp=.o)
OBJECTS = $(addprefix $(OBJDIR), $(OBJ:.c=.o)) 

UNICSOURCES = $(addprefix $(COREDIR), game.cpp graphics.cpp input.cpp)

OBJS = $(addprefix $(OBJDIR), $(UNICSOURCES:.cpp=.o)) $(OBJECTS)

UNIQHEADERS = $(SCRIPTSDIR)LuaScriptConfig.h $(UNITSDIR)YOBA.h $(MAPDIR)Waypoint.h \
         	$(addprefix $(3RDPARTYDIR), TypeList.h timer/InternalTimerEvent.h \
         	timer/InternalTimerEvent.h timer/TimerEvent.h timer/ITimerEventPerformer.h ) \
         	Define.h debug.h hacks.h safestring.h basic_types.h types.h


HEADERS = $(OBJECTS:.o=.h) $(addprefix $(OBJDIR), $(UNIQHEADERS))
 
 
GCHOLD = $(HEADERS:.h=.h.gch)
GCH = $(shell echo $(GCHOLD) | sed -e "s/[^ ]\+\/\(main\|LuaRegister\|api\/Widgets\|api\/Units\).h.gch //g")


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
	$(CC) $(CFLAGS)  -o $(OUTDIR)$(PROGNAME) $(OBJS) $(LIBS)

$(OBJDIR):
	mkdir -p $(addprefix $(OBJDIR), $(COREDIR) $(SCRIPTSDIR) $(SCRIPTSDIR)$(SCRIPTSAPIDIR) \
	 $(GRAPHICSDIR) $(GRAPHICSDIR)render/ $(GRAPHICSDIR)sprite/ $(GRAPHICSDIR)utils/  \
	 $(UNITSDIR) $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR) $(MAPDIR) $(3RDPARTYDIR) \
	 $(3RDPARTYDIR)timer $(PHYSICSDIR))


clean: cleanheaders cleanobjs cleanprog cleandirs

cleanheaders:
	$(rm) $(GCH)

cleanobjs:
	$(rm) $(OBJS)

cleanprog:
	$(rm) $(OUTDIR)$(PROGNAME)

cleandirs:
	$(rm) -r $(OBJDIR)
