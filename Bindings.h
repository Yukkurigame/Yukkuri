/*
 * Bindings.h
 *
 *  Created on: 07.07.2010
 */

#ifndef BINDINGS_H_
#define BINDINGS_H_

#include "types.h"

#include "BindFunctions.h"
#include "SDL/SDL_keysym.h"


#define MAXKEYS SDLK_LAST+20

enum func_t { NOTAFUNC=0, CFUNC, LUAFUNC };

struct BindFunction {
	enum func_t type;
	BindCFunction cref;
	LuaRegRef luaref;
};


namespace Bindings
{
	void init();

	void process( int num, short down, UINT16 unicode );

	void bindKey( int key, const char* name );

	void bindCFunction( int key, UINT funcname );
	void bindLuaFunction( int key, LuaRegRef func );

	void loadKeys( const char* );
	void freeKeys( );
	const char* getCurrent( );
	const char* keyName( UINT i );

	LuaRegRef& getReciever( );


};



#endif /* BINDINGS_H_ */
