
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

UTILS = bson.cpp list.cpp path.cpp utf.cpp
CORE = yukkuri.cpp
UNITS =  unitmanager.cpp ProtoStack.cpp Prototypes.cpp ActionTimer.cpp Scope.cpp Unit.cpp \
		 UnitStatic.cpp UnitDynamic.cpp UnitEntity.cpp UnitCorpse.cpp UnitPlayer.cpp
PHYSICS = physics.cpp handlers.cpp
RENDER = Atlas.cpp GBuffer.cpp GLHelpers.cpp GLTextures.cpp VBuffer.cpp Textures.cpp
SPRITE = Sprite.cpp Animation.cpp AnimationDefines.cpp Brush.cpp Material.cpp Mesh.cpp
GRAPHUTILS = gl_shader.cpp gl_uniforms.cpp VBOArray.cpp Image.cpp ElasticBox.cpp Primitives.cpp
GRAPHICS = Camera.cpp Font.cpp gl_extensions.cpp Lighting.cpp Render.cpp Text.cpp \
		   $(addprefix render/, $(RENDER)) $(addprefix sprite/, $(SPRITE)) $(addprefix utils/, $(GRAPHUTILS))
PUSHERAPI = TextureProxy.cpp ShaderConfigData.cpp GraphicsTypes.cpp Tiles.cpp
SCRIPTSAPI = $(addprefix modules/, LightsApi.cpp UnitManagerApi.cpp InterfaceApi.cpp	\
			 ThreadManagerApi.cpp  CameraApi.cpp BindingsApi.cpp PathsApi.cpp RegionApi.cpp) \
			 Widgets.cpp Units.cpp  YOBA.cpp $(addprefix $(PUSHERAPIDIR), $(PUSHERAPI))
SCRIPTS = Lua.cpp LuaRegister.cpp LuaConfig.cpp LuaScript.cpp LuaThread.cpp proto.cpp \
		  api.cpp $(addprefix $(SCRIPTSAPIDIR), $(SCRIPTSAPI))
WIDGETS = Widget.cpp WidgetText.cpp WidgetBar.cpp
INTERFACE = Interface.cpp $(addprefix $(WIDGETSDIR), $(WIDGETS))
MAP = $(addsuffix .cpp, Tiles Chunk Region RegionFile RegionFileCache Map \
	$(addprefix generator/, MapGen MapGenerator Dumper NoisyEdges Watersheds IslandShape \
		$(addprefix graph/, Center Corner Edge) ) )
3RDPARTY = CUData.cpp CUDataUser.cpp CUDataTemplates.cpp LuaPusher.cpp rand31.cpp timer/TimerManager.cpp \
		   $(addprefix objTester/, list.cpp obj_parser.cpp string_extra.cpp) \
		   $(addprefix bson/, bson.c encoding.c ) 

SRCS =   main.cpp config.cpp Bindings.cpp BindFunctions.cpp debug.cpp \
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
	 	$(SCRIPTSDIR)LuaScriptConfig $(UNITSDIR)YOBA $(UTILSDIR)misc \
	 	$(addprefix $(MAPDIR), Waypoint generator/generator_constants generator/Histogram ) \
		$(addprefix $(3RDPARTYDIR), TypeList \
			$(addprefix timer/, InternalTimerEvent InternalTimerEvent TimerEvent \
			ITimerEventPerformer )) \
		 Define hacks safestring basic_types types )


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
	 $(SCRIPTSDIR)$(SCRIPTSAPIDIR)$(PUSHERAPIDIR) $(SCRIPTSDIR)$(SCRIPTSAPIDIR)/modules \
	 $(GRAPHICSDIR) $(GRAPHICSDIR)render/ $(GRAPHICSDIR)sprite/ $(GRAPHICSDIR)utils/ $(UNITSDIR) \
	 $(INTERFACEDIR) $(INTERFACEDIR)$(WIDGETSDIR) $(MAPDIR) $(MAPDIR)/generator \
	 $(MAPDIR)/generator/graph $(MAPDIR)/generator/fifth-party $(3RDPARTYDIR) $(3RDPARTYDIR)objTester \
	 $(3RDPARTYDIR)timer $(3RDPARTYDIR)voronoi $(PHYSICSDIR))


clean: cleanheaders cleanobjs cleanprog cleandirs


cleanheaders:
	$(rm) $(GCH)

cleanobjs:
	$(rm) $(OBJS)

cleanprog:
	$(rm) $(OUTDIR)$(PROGNAME)

cleandirs:
	$(rm) -r $(OBJDIR)*
