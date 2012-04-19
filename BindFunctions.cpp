/*
 * BindFunctions.cpp
 *
 *  Created on: 17.04.2012
 *
 */

#include "BindFunctions.h"
#include "yukkuri.h"
#include "unitmanager.h"
#include "Player.h"
#include "sdl_graphics.h"

#include "debug.h"

#define FUNCTIONSCOUNT 9

static const std::string func_names[FUNCTIONSCOUNT] = {
	"up", "down", "left", "right", "toggleinterface",
	"attack", "eat", "exit", "screenshot"
};

static const BindCFunction func_refs[FUNCTIONSCOUNT] = {
	&Binds::movePlayerUp, &Binds::movePlayerDown, &Binds::movePlayerLeft,
	&Binds::movePlayerRight, &Binds::playerToggleInterface, &Binds::playerAttackUnit,
	&Binds::playerEat, &Binds::exit, &Binds::screenshot
};


int Binds::getFunction( std::string name, BindCFunction* f )
{
	for( int i = 0; i < FUNCTIONSCOUNT; ++i){
		if( func_names[i] == name ){
			*f = func_refs[i];
			return 1;
		}
	}
	Debug::debug( Debug::INPUT, "Bind function not found: " + name + ".\n" );

	return 0;
}

void Binds::movePlayerUp( short down )
{
	Unit* player;
	player = UnitManager::units.GetPlayer();
	if( player )
		player->moveUnit( 0, down );
}

void Binds::movePlayerDown( short down )
{
	Unit* player;
	player = UnitManager::units.GetPlayer();
	if( player )
		player->moveUnit( 0, down * -1 );
}

void Binds::movePlayerLeft( short down )
{
	Unit* player;
	player = UnitManager::units.GetPlayer();
	if( player )
		player->moveUnit( 1, down * -1 );
}

void Binds::movePlayerRight( short down )
{
	Unit* player;
	player = UnitManager::units.GetPlayer();
	if( player )
		player->moveUnit( 1, down );
}

void Binds::playerAttackUnit( short down )
{
	Player* player;
	player = dynamic_cast<Player*>(UnitManager::units.GetPlayer());
	if( player )
		player->attack( );
}

void Binds::playerToggleInterface( short down )
{
	Player* player;
	player = dynamic_cast<Player*>(UnitManager::units.GetPlayer());
	if( player )
		player->toggleInterface();
}

void Binds::playerEat( short down )
{
	Player* player;
	player = dynamic_cast<Player*>(UnitManager::units.GetPlayer());
	if( player )
		player->eat( );
}

void Binds::screenshot( short down )
{
	Screenshot::Save( );
}

void Binds::exit( short down )
{
	extern Yukkuri Engine;
	Engine.Quit();
}

