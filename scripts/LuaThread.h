/*
 * LuaThread.h
 *
 *  Created on: 31.10.2010
 */

#ifndef LUATHREAD_H_
#define LUATHREAD_H_

#include "Lua.h"

//FIXME: Struct maybe
struct LuaThread
{
	unsigned int ThreadId;
	lua_State* Thread;
	bool Pausable;

	LuaThread( );
	LuaThread( lua_State* self );
};

namespace threadsManager
{
	unsigned int NewThread( lua_State* L );
	unsigned int GetThread( lua_State* L );
	int ThreadWait( lua_State *L );
	int ResumeThread( lua_State *L );
	bool RemoveThread( unsigned int id );
	void CleanThreads( );

}

#endif /* LUATHREAD_H_ */
