/*
 * BindFunctions.cpp
 *
 *  Created on: 17.04.2012
 *
 */

#include "BindFunctions.h"

#include "core/yukkuri.h"
#include "units/unitmanager.h"
#include "units/UnitPlayer.h"
#include "graphics/utils/Image.h"

#include "debug.h"


static const BindCFunction func_refs[Binds::bndLast] = {
	&Binds::dummy, &Binds::movePlayerUp, &Binds::movePlayerDown,
	&Binds::movePlayerLeft, &Binds::movePlayerRight,
	&Binds::playerAttackUnit, &Binds::playerEat, &Binds::exit,
	&Binds::screenshot
};


int Binds::getFunction( enum func_numbers name, BindCFunction* f )
{
	if( name == bndNone )
		Debug::debug( Debug::INPUT, "Bind void function.\n" );
	if( name < bndLast ){
		*f = func_refs[name];
		return 1;
	}
	Debug::debug( Debug::INPUT, "Bind function not found: " + std::string(getFunctionName(name)) + ".\n" );

	return 0;
}


void Binds::movePlayerUp( short down )
{
	Unit* player;
	player = UnitManager::GetPlayer();
	if( player )
		player->moveUnit( 0, down );
}

void Binds::movePlayerDown( short down )
{
	Unit* player;
	player = UnitManager::GetPlayer();
	if( player )
		player->moveUnit( 0, down * -1 );
}

void Binds::movePlayerLeft( short down )
{
	Unit* player;
	player = UnitManager::GetPlayer();
	if( player )
		player->moveUnit( 1, down * -1 );
}

void Binds::movePlayerRight( short down )
{
	Unit* player;
	player = UnitManager::GetPlayer();
	if( player )
		player->moveUnit( 1, down );
}

void Binds::playerAttackUnit( short down )
{
	/*if( down )
		return;
	Player* player;
	player = dynamic_cast<Player*>(UnitManager::GetPlayer());
	if( player )
		player->attack( );
	*/
}

void Binds::playerEat( short down )
{
	/*
	if( down )
		return;
	Player* player;
	player = dynamic_cast<Player*>(UnitManager::GetPlayer());
	if( player )
		player->eat( );
	*/
}

void Binds::screenshot( short down )
{
	Image::save( );
}

void Binds::exit( short down )
{
	Yukkuri::End();
}

