/*
 * yukkuri.cpp
 *
 *  Created on: 19.04.2012
 *
 */
#include "yukkuri.h"
#include "Render.h"
#include "Interface.h"
#include "Camera.h"
#include "Text.h"
#include "map.h"
#include "debug.h"
using namespace Debug;

#include "LuaScript.h"
#include "LuaThread.h"

Yukkuri Engine;

bool Yukkuri::Init()
{
	LuaScript* ls = new LuaScript;

	if( !ls->Init( ) || !ls->OpenFile( "init" ) ){
		debug( MAIN, "Lua loading failed.\n" );
		return false;
	}

	delete ls;

	return CEngine::Init( );
}

bool Yukkuri::AdditionalInit()
{
	extern Map map;

	debug( MAIN, "Additional Init\n" );

	//Bindings::bnd.LoadKeys( "" );

	map.LoadTiles( );
	map.Init( );

	LuaScript* ls = new LuaScript;

	if( !ls->OpenFile( "start" ) ){
		debug( SCRIPT, "Starting lua failed.\n" );
		return false;
	}

	delete ls;

	daytime.loadInterface();

	units = &UnitManager::units;

	Widget* w = UI::yui.GetWidget("fps");
	if( w )
		w->bindValue( &CurrentFPS );

	return true;
}

void Yukkuri::Think( const int& ElapsedTime )
{
	// Do time-based calculations
	units->tick( ElapsedTime );
	daytime.update( ElapsedTime );
	UI::yui.Update( );
	spawner.Spawn( );
}

void Yukkuri::Render( )
{
	extern Map map;

	RenderManager::Instance()->CleanGLScene( );

	YCamera::CameraControl.Update( );

	// Display slick graphics on screen
	units->onDraw( );

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
	RenderManager::Destroy();
	CleanFonts();
	ftDone();
	threadsManager::CleanThreads();

	Debug::debug( Debug::MAIN, "Done.\n" );
}



