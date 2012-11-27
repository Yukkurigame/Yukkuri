/*
 * yukkuri.cpp
 *
 *  Created on: 19.04.2012
 *
 */
#include "core/yukkuri.h"
#include "interface/Interface.h"
#include "3rdparty/timer/TimerManager.h"
#include "map/Map.h"
#include "map/Region.h"
#include "scripts/LuaScript.h"
#include "scripts/LuaThread.h"
#include "scripts/proto.h"
#include "units/unitmanager.h"
#include "Bindings.h"
#include "config.h"
#include "daytime.h"
#include "debug.h"
#include "misc.h"


namespace {
	enum GameState last_state;
}


float currentFPS;


bool Yukkuri::Init()
{
	extern LuaScript* luaScript;
	extern MainConfig conf;
	Window.title = NULL;
	Window.minimized = false;
	Window.state = gsLoading;
	currentFPS = 0.0;

	Paths::init();

	// Load key names for register in api
	Bindings::init();

	// Init script system
	if( !luaScript->init( ) || !luaScript->OpenFile( "init" ) ){
		Debug::debug( Debug::MAIN, "Lua loading failed.\n" );
		return false;
	}

	// Load default scripts
	Debug::debug( Debug::MAIN, "Loading defaults.\n" );
	if( !conf.load( ) ){
		Debug::debug( Debug::MAIN, "Loading default configuration failed. Exiting." );
		return false;
	}

	return InitGraphics() && InitInput() && AdditionalInit();
}


bool Yukkuri::AdditionalInit()
{
	extern LuaScript* luaScript;

	Debug::debug( Debug::MAIN, "Additional Init\n" );

	UnitManager::init( );
	Region::init();
	Map::init( );

	DayTime::init();

	if( !luaScript->OpenFile( "start" ) ){
		Debug::debug( Debug::SCRIPT, "Starting lua failed.\n" );
		return false;
	}

	DayTime::loadInterface();

	Widget* w = Interface::GetWidget( "fps", NULL );
	if( w )
		w->bindValue( &(currentFPS) );

	return true;
}

void Yukkuri::Think( const int& ElapsedTime )
{
	// Do time-based calculations
	UnitManager::tick( ElapsedTime );
	Interface::Update( );
}


void Yukkuri::WindowInactive( )
{
	last_state = Window.state;
	Window.state = gsPaused;
};
void Yukkuri::WindowActive( )
{
	Window.state = last_state;
};


void Yukkuri::Clean()
{
	//Clear timer
	Timer::DeleteAllEvents();

	// Clear interface
	Interface::clean( );

	// Clear units
	UnitManager::clean( );
	clean_prototypes();

	// Clear other
	Map::clean( );
	Region::clean( );
	DayTime::clean();

	threadsManager::CleanThreads( );

	CleanGraphics();

	Debug::debug( Debug::MAIN, "Done.\n" );
}
