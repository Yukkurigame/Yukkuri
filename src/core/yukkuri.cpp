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
#include "utils/path.h"
#include "Bindings.h"
#include "config.h"
#include "debug.h"
#include "time.h"


namespace {
	enum GameState last_state;
}


float currentFPS;

// Gcc bug
extern LuaScript* luaScript;


bool Yukkuri::Init()
{
	Window.title = NULL;
	Window.minimized = false;
	Window.state = gsLoading;
	currentFPS = 0.0;

	srand(time(NULL));

	Path::init();

	// Load key names for register in api
	Bindings::init();

	// Init script system
	if( !luaScript->init( ) || !luaScript->OpenFile( "init" ) ){
		Debug::debug( Debug::MAIN, "Lua loading failed.\n" );
		return false;
	}

	// Load default scripts
	Debug::debug( Debug::MAIN, "Loading defaults.\n" );
	if( !loadMainConfig( ) ){
		Debug::debug( Debug::MAIN, "Loading default configuration failed. Exiting." );
		return false;
	}

	return InitGraphics() && InitInput() && AdditionalInit();
}


bool Yukkuri::AdditionalInit()
{
	Debug::debug( Debug::MAIN, "Additional Init\n" );

	UnitManager::init( );
	Region::init( );
	Map::init( );

	if( !luaScript->OpenFile( "start" ) ){
		Debug::debug( Debug::SCRIPT, "Starting lua failed.\n" );
		return false;
	}

	Interface::init( );

	return true;
}

void Yukkuri::Think( const int& ElapsedTime )
{
	// Do time-based calculations
	UnitManager::tick( ElapsedTime );

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

	threadsManager::CleanThreads( );

	CleanGraphics();

	Debug::debug( Debug::MAIN, "Done.\n" );
}
