#include <stdlib.h>
#include "yukkuri.h"


// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
	Yukkuri Engine;

	if( Engine.Init() ){
		Engine.SetTitle( "Yukkuri game" );
		Engine.Start();
		Engine.SetTitle( "Quitting..." );
		return 1;
	}

	return 0;
}


bool Yukkuri::Init()
{
	ls = new LuaScript;

	if( !ls->Init( ) || !ls->OpenFile( "init" ) ){
		debug( 1, "Lua loading failed.\n" );
		return false;
	}

	return CEngine::Init( );
}

bool Yukkuri::AdditionalInit()
{
	extern Map map;

	debug( 1, "Additional Init\n" );

	if( !ls->OpenFile( "start" ) ){
		debug( 1, "Starting lua failed.\n" );
		return false;
	}

	Bindings::bnd.setEngine( this );

	Bindings::bnd.LoadKeys();

	map.LoadTiles( );
	map.Init( );

	daytime.loadInterface();

	units = &UnitManager::units;

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

	Graphics::Instance()->CleanGLScene( );

	YCamera::CameraControl.Update( );

	// Display slick graphics on screen
	units->DrawUnits( );

	map.Draw( );

	daytime.draw( );

	UI::yui.GetWidget("fps")->setText( GetFPSText() );

	//Draw to screen
	Graphics::Instance()->DrawGLScene( );
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
	//FIXME: графика уничтожается, а потом начинают уничтожаться спрайты, да.
	//теги: ололо-сегфолты
	//Graphics::Destroy();
}
