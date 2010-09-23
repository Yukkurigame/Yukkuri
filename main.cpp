#include <stdlib.h>
#include "yukkuri.h"


// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
	Yukkuri Engine;

	Engine.Init();

	Engine.SetTitle( "Yukkuri game" );
	Engine.Start();

	Engine.SetTitle( "Quitting..." );

	return 0;
}


void Yukkuri::AdditionalInit()
{
	//map.Init("map.map");
	debug( 1, "Additional Init\n" );

	LuaScript* s = new LuaScript( );
	if( !s->Init( ) || !s->OpenFile( "init" ) )
		debug( 1, "Lua loading failed.\n" );

	Bindings::bnd.setEngine( this );

	Bindings::bnd.LoadKeys();

	daytime.loadInterface();

	units = &UnitManager::units;
	YCamera::CameraControl.SetTarget( units->GetPlayer()->getUnitpX(), units->GetPlayer()->getUnitpY());

}

void Yukkuri::Think( const int& ElapsedTime )
{
	// Do time-based calculations
	units->tick( ElapsedTime );
	daytime.update( ElapsedTime );
	spawner.Spawn();
}

void Yukkuri::Render( )
{
	Graphics::Instance()->CleanGLScene( );

	// Display slick graphics on screen
	units->DrawUnits( );

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
	LuaConfig::Destroy();
	//FIXME: графика уничтожается, а потом начинают уничтожаться спрайты, да.
	//теги: ололо-сегфолты
	//Graphics::Destroy();
}
