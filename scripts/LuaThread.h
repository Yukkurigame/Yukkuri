/*
 * LuaThread.h
 *
 *  Created on: 31.10.2010
 */

#ifndef LUATHREAD_H_
#define LUATHREAD_H_

#include "Lua.h"
#include <list>
#include "types.h"

//FIXME: Struct maybe
struct LuaThread
{
	unsigned int ThreadId;
	lua_State* Thread;
	LuaRegRef refKey;
	bool Pausable;

	LuaThread( );
	LuaThread( lua_State* self );
};

typedef std::list< LuaThread* >::iterator ThreadIter;

namespace threadsManager
{
	LuaRegRef NewThread( lua_State* L );
	ThreadIter GetThread( lua_State* L );
	ThreadIter End( );
	void ProcessThread( LuaRegRef r );
	int ThreadWait( lua_State *L );
	int ResumeThread( ThreadIter it, lua_State *L );
	void RemoveThread( ThreadIter it );
	void CleanThreads( );

}

#endif /* LUATHREAD_H_ */
