/*
 * LuaScript.h
 *
 *  Created on: 11.09.2010
 */

#ifndef LUASCRIPT_H_
#define LUASCRIPT_H_

#include "Lua.h"
#include "types.h"

class LuaScript: public LuaMain
{
public:
	LuaScript();
	bool Init();
	bool OpenFile( std::string name );

	LuaRegRef AddToRegistry();
	void RemoveFromRegistry( LuaRegRef& r );
	void GetFromRegistry( lua_State* L, LuaRegRef r );

	void RegProc( lua_State* L, LuaRegRef* procref, int n );
	void ReserveProc( LuaRegRef procref );
	void ReleaseProc( LuaRegRef* procref );

	int ExecChunk();
	int ExecChunkFromReg( LuaRegRef r) ;

	static void StackDumpToLog( lua_State *L );
};

#endif /* LUASCRIPT_H_ */
