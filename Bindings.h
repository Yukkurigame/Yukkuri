/*
 * Bindings.h
 *
 *  Created on: 07.07.2010
 */

#ifndef BINDINGS_H_
#define BINDINGS_H_

#include "types.h"

#include <string>
#include <map>

#include "BindFunctions.h"
#include "SDL/SDL_keysym.h"


#define MAXKEYS SDLK_LAST+20

enum func_t { NOTAFUNC=0, CFUNC, LUAFUNC };

struct BindFunction {
	enum func_t type;
	BindCFunction cref;
	LuaRegRef luaref;
};


class Bindings
{
public:
	Bindings();

	static Bindings bnd;

	void process( int num, short down );

	void BindKey( int key, std::string name );
	void unBindKey( std::string name );

	void BindCFunction( int key, std::string funcname );
	void BindLuaFunction( int key, LuaRegRef func );

	void LoadKeys( std::string );

private:
	std::map< std::string, int > Keys;
	BindFunction BindedFunctions[ MAXKEYS ];
};



#endif /* BINDINGS_H_ */
