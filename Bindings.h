/*
 * Bindings.h
 *
 *  Created on: 07.07.2010
 */

#ifndef BINDINGS_H_
#define BINDINGS_H_

#include "debug.h"

#include <string>
#include <map>

//FIXME: это все как-то криво. Прямо FUUUUUUUUUUUUUUUU~
//FUFUFU
//FUFUFUFUUUUUUU

class Bindings
{
public:
	Bindings();

	static Bindings bnd;

	void process( int num, short down );

	void BindKey( int key, std::string name );
	void unBindKey( int key );

	void LoadKeys();

	void setEngine( const void* e ) { engine = e; }

private:
	const void* engine;
	std::map< int, std::string> Keys;
	std::map< std::string, std::string> Bindkeys;
};

//TODO: Как-то нехорошо.
namespace Binds
{
	void movePlayerUp( short down );
	void movePlayerDown( short down );
	void movePlayerLeft( short down );
	void movePlayerRight( short down );
	void playerAttackUnit( );
	void playerToggleInterface( );
	void playerEat(  );
	void screenshot( );
	void exit( const void* e );
}


#endif /* BINDINGS_H_ */
