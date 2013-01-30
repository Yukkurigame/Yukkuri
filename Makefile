
include Makefile.in


OBJDIR= obj/
OUTDIR= bin/
VPATH= src/
PROGNAME= Yukkuri
COREDIR= core/
SCRIPTSAPIDIR= api/
PUSHERAPIDIR = pusher/
SCRIPTSDIR= scripts/
UNITSDIR= units/
PHYSICSDIR= $(UNITSDIR)physics/
GRAPHICSDIR= graphics/
WIDGETSDIR= widgets/
INTERFACEDIR= interface/
MAPDIR = map/
3RDPARTYDIR= 3rdparty/
UTILSDIR = utils/

UTILS = list.cpp
CORE = yukkuri.cpp
UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp ActionTimer.cpp Scope.cpp Unit.cpp \
		 UnitStatic.cpp UnitDynamic.cpp UnitEntity.cpp UnitCorpse.cpp UnitPlayer.cpp
PHYSICS = physics.cpp handlers.cpp
RENDER = Atlas.cpp GBuffer.cpp GLHelpers.cpp GLTextures.cpp VBuffer.cpp Textures.cpp
SPRITE = Sprite.cpp Animation.cpp AnimationDefines.cpp Brush.cpp Material.cpp
GRAPHUTILS = gl_shader.cpp gl_uniforms.cpp VBOArray.cpp Image.cpp ElasticBox.cpp
GRAPHICS = Camera.cpp daytime.cpp Font.cpp gl_extensions.cpp Lighting.cpp Render.cpp Text.cpp \
		   $(addprefix render/, $(RENDER)) $(addprefix sprite/, $(SPRITE)) $(addprefix utils/, $(GRAPHUTILS))
PUSHERAPI = TextureProxy.cpp
SCRIPTSAPI = LightsApi.cpp UnitManagerApi.cpp InterfaceApi.cpp Widgets.cpp ThreadManagerApi.cpp \
			 CameraApi.cpp BindingsApi.cpp Units.cpp PathsApi.cpp RegionApi.cpp YOBA.cpp \
			 $(addprefix $(PUSHERAPIDIR), $(PUSHERAPI))
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp LuaPusher.cpp \
		  proto.cpp api.cpp $(addprefix $(SCRIPTSAPIDIR), $(SCRIPTSAPI))
WIDGETS = Widget.cpp WidgetText.cpp WidgetBar.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))
MAP = Tiles.cpp Region.cpp Map.cpp
3RDPARTY = CUData.cpp CUDataUser.cpp CUDataTemplates.cpp LuaPusher.cpp timer/TimerManager.cpp



SRCS =   main.cpp config.cpp misc.cpp Bindings.cpp BindFunctions.cpp utf.cpp \
		 $(addprefix $(UTILSDIR), $(UTILS)) \
         $(addprefix $(COREDIR), $(CORE)) \
         $(addprefix $(SCRIPTSDIR), $(SCRIPTS)) \
         $(addprefix $(PHYSICSDIR), $(PHYSICS)) \
         $(addprefix $(UNITSDIR), $(UNITS)) \
         $(addprefix $(GRAPHICSDIR), $(GRAPHICS)) \
         $(addprefix $(INTERFACEDIR), $(INTERFACE)) \
         $(addprefix $(MAPDIR), $(MAP)) \
         $(addprefix $(3RDPARTYDIR), $(3RDPARTY))
         


OBJ = $(SRCS:.cpp=.o)
OBJECTS = $(addprefix $(OBJDIR), $(OBJ:.c=.o)) 

UNICSOURCES = $(addprefix $(COREDIR), game.cpp graphics.cpp input.cpp)

OBJS = $(addprefix $(OBJDIR), $(UNICSOURCES:.cpp=.o)) $(OBJECTS)

UNIQHEADERS = $(addsuffix .h, \
	 	$(SCRIPTSDIR)LuaScriptConfig $(UNITSDIR)YOBA $(MAPDIR)Waypoint \
		$(addprefix $(3RDPARTYDIR), TypeList \
			$(addprefix timer/, InternalTimerEvent InternalTimerEvent TimerEvent \
			ITimerEventPerformer )) \
		 Define debug hacks safestring basic_types types )


HEADERS = $(OBJECTS:.o=.h) $(addprefix $(OBJDIR), $(UNIQHEADERS))
MISSINGHEADERS = $(addprefix $(OBJDIR), $(addsuffix .h, main \
		$(addprefix $(SCRIPTSDIR), LuaRegister LuaPusher \
			$(addprefix $(SCRIPTSAPIDIR), Widgets Units YOBA pusher/TextureProxy)	\
		)))

GHEADERS = $(filter-out $(MISSINGHEADERS), $(HEADERS)) 
GCH = $(GHEADERS:.h=.h.gch)


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



.PHONY: all clean

all: $(PROGNAME)

rebuild: cleanprog all

$(PROGNAME) : dirs $(OBJS)	
	$(CC) $(CFLAGS)  -o $(OUTDIR)$(PROGNAME) $(OBJS) $(LIBS)


# Precompiled headers ~ 800mb on x86_64 and 400 on x86
pch: $(GCH)


dirs:
	mkdir -p $(addprefix $(OBJDIR), $(UTILSDIR) $(COREDIR) $(SCRIPTSDIR) 	\
	 $(SCRIPTSDIR)$(SCRIPTSAPIDIR)$(PUSHERAPIDIR) $(GRAPHICSDIR) $(GRAPHICSDIR)render/		\
	 $(GRAPHICSDIR)sprite/ $(GRAPHICSDIR)utils/ $(UNITSDIR)					\
	 $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR) $(MAPDIR) $(3RDPARTYDIR) 	\
	 $(3RDPARTYDIR)timer $(PHYSICSDIR))


clean: cleanheaders cleanobjs cleanprog cleandirs


cleanheaders:
	$(rm) $(GCH)

cleanobjs:
	$(rm) $(OBJS)

cleanprog:
	$(rm) $(OUTDIR)$(PROGNAME)

cleandirs:
	$(rm) -r $(OBJDIR)*
