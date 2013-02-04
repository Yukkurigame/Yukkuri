/*
 * BindFunctions.h
 *
 *  Created on: 17.04.2012
 *
 */

#ifndef BINDFUNCTIONS_H_
#define BINDFUNCTIONS_H_


typedef void (*BindCFunction)( short );


namespace Binds
{
	enum func_numbers {
		bndNone=0, bndUp, bndDown, bndLeft, bndRight, bndAttack, bndEat, bndExit, bndScreenshot,
		bndLast
	};

	static const char* func_names[bndLast] = {
		"none", "up", "down", "left", "right", "attack", "eat", "exit", "screenshot",
	};

	inline void dummy( short down ) { };

	void movePlayerUp( short down );
	void movePlayerDown( short down );
	void movePlayerLeft( short down );
	void movePlayerRight( short down );
	void playerAttackUnit( short down );
	void playerEat( short down  );
	void screenshot( short down );
	void exit( short down );

	int getFunction( enum func_numbers name, BindCFunction* f );
	inline const char* getFunctionName( enum func_numbers name ){
		if( name < bndLast )
			return func_names[name];
		return "";
	}

};



#endif /* BINDFUNCTIONS_H_ */
