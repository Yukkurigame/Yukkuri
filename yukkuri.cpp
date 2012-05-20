/*
 * yukkuri.cpp
 *
 *  Created on: 19.04.2012
 *
 */
#include "yukkuri.h"
#include "graphics/Text.h"
#include "graphics/Render.h"
#include "interface/Interface.h"
#include "units/unitmanager.h"
#include "Camera.h"
#include "map.h"
#include "debug.h"
using namespace Debug;

#include "LuaScript.h"
#include "LuaThread.h"

#include "hacks.h"


Yukkuri Engine;

bool Yukkuri::Init()
{
	extern LuaScript* luaScript;

	if( !luaScript->Init( ) || !luaScript->OpenFile( "init" ) ){
		debug( MAIN, "Lua loading failed.\n" );
		return false;
	}

	return CEngine::Init( );
}

bool Yukkuri::AdditionalInit()
{
	extern Map map;
	extern LuaScript* luaScript;

	debug( MAIN, "Additional Init\n" );

	map.LoadTiles( );
	map.Init( );

	if( !luaScript->OpenFile( "start" ) ){
		debug( SCRIPT, "Starting lua failed.\n" );
		return false;
	}

	daytime.loadInterface();

	Widget* w = Interface::GetWidget( "fps", NULL );
	if( w )
		w->bindValue( &CurrentFPS );

	return true;
}

void Yukkuri::Think( const int& ElapsedTime )
{
	// Do time-based calculations
	UnitManager::tick( ElapsedTime );
	daytime.update( ElapsedTime );
	Interface::Update( );
	spawner.Spawn( );
}

void Yukkuri::Render( )
{
	extern Map map;

	RenderManager::Instance()->CleanGLScene( );

	YCamera::CameraControl.Update( );

	// Display slick graphics on screen
	map.onDraw( );

	daytime.onDraw( );

	//Draw to screen
	RenderManager::Instance()->DrawGLScene( );
}

void Yukkuri::MouseMoved( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY )
{
	// Handle mouse movement

	// X and Y are absolute screen positions
	// RelX and RelY are screen position relative to last detected mouse movement
}

void Yukkuri::MouseButtonUp( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY )
{
	// Handle mouse button released
}

void Yukkuri::MouseButtonDown( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY )
{
	// Handle mouse button pressed
}

void Yukkuri::WindowInactive()
{
	// Pause game
}

void Yukkuri::WindowActive()
{
	// Un-pause game
}

void Yukkuri::End()
{
	extern std::vector<Proto*> Prototypes;
	clear_vector( &Prototypes );

	Interface::clean( );

	UnitManager::clean( );

	threadsManager::CleanThreads( );

	RenderManager::Destroy( );
	CleanFonts();
	ftDone();

	Debug::debug( Debug::MAIN, "Done.\n" );
}



