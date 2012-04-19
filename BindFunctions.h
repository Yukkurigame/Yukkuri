/*
 * BindFunctions.h
 *
 *  Created on: 17.04.2012
 *
 */

#ifndef BINDFUNCTIONS_H_
#define BINDFUNCTIONS_H_

#include <map>
#include <string>

typedef void (*BindCFunction)( short );

namespace Binds
{
	void movePlayerUp( short down );
	void movePlayerDown( short down );
	void movePlayerLeft( short down );
	void movePlayerRight( short down );
	void playerAttackUnit( short down );
	void playerToggleInterface( short down );
	void playerEat( short down  );
	void screenshot( short down );
	void exit( short down );

	int getFunction( std::string name, BindCFunction* f );


};



#endif /* BINDFUNCTIONS_H_ */
