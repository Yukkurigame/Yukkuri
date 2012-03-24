/*
 * LuaScript.h
 *
 *  Created on: 11.09.2010
 */

#ifndef LUASCRIPT_H_
#define LUASCRIPT_H_

#include "Lua.h"

class LuaScript: public LuaMain
{
public:
	LuaScript();
	bool Init();
	void RegisterApi( );
	bool OpenFile( std::string name );
};

#endif /* LUASCRIPT_H_ */
