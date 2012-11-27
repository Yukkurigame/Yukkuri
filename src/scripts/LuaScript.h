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

	bool OpenFile( std::string name );

	LuaRegRef AddToRegistry();
	void RemoveFromRegistry( LuaRegRef& r );
	void GetFromRegistry( lua_State* L, LuaRegRef r );

	void RegProc( lua_State* L, LuaRegRef* procref, int n );
	void RegObject( lua_State* L, LuaRegRef* procref, int n );
	void ReserveObject( LuaRegRef procref );
	void ReleaseObject( LuaRegRef* procref );

	int ExecChunk( UINT args = 0 );
	int ExecChunkFromReg( LuaRegRef r, UINT args = 0 );

	static void StackDumpToLog( lua_State *L );
};

#endif /* LUASCRIPT_H_ */
