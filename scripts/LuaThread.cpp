/*
 * LuaThread.cpp
 *
 *  Created on: 31.10.2010
 */

#include "LuaThread.h"
#include "debug.h"
#include <list>

static int threadsCounter = 0;

static std::list<LuaThread*> threads;

static string getDebugInfo( lua_State* L )
{
	lua_Debug ar;
	//FIXME: it's cruve.
	char d[65];
	lua_getinfo(L, ">S", &ar);
	sprintf(d, "%s line %d", ar.short_src, ar.linedefined );
	return (string)d;
}

LuaThread::LuaThread( )
{
	ThreadId = ++threadsCounter;
	Thread = NULL;
	Pausable = false;
}

LuaThread::LuaThread( lua_State* self )
{
	ThreadId = ++threadsCounter;
	Thread = self;
	Pausable = false;
}

unsigned int threadsManager::NewThread( lua_State* L )
{
	lua_State *NL;
	LuaThread* thread;
	bool pausable;
	if( !L )
		return 0;
	if( lua_gettop( L ) < 1 || lua_gettop( L ) > 2 ){
		debug(3, "Wrong arguments count passed:" + getDebugInfo( L ) + ".\n");
		return 0;
	}

	pausable = false;
	if( lua_gettop(L) == 2 ){
		pausable = lua_toboolean(L, 2);
		lua_pop(L, 1);
	}

	if( !lua_isfunction( L, 1 ) || lua_iscfunction( L, 1 ) ){
		debug(3, "Wrong first argument passed:" + getDebugInfo( L ) + ". Function expected.\n");
		return 0;
	}

	NL = lua_newthread(L);

	lua_pushvalue(L, 1);
	lua_xmove(L, NL, 1);

	thread = new LuaThread( NL );

	thread->Pausable = pausable;

	threads.push_back( thread );

	return thread->ThreadId;
}

unsigned int threadsManager::GetThread( lua_State* L )
{
	for( std::list<LuaThread*>::iterator it = threads.begin(), end = threads.end(); it != end; ++it ){
		if( (*it)->Thread == L )
			return (*it)->ThreadId;
	}
	return 0;
}

int threadsManager::ResumeThread( lua_State *L )
{
	LuaThread* thread = NULL;
	lua_State* co = lua_tothread( L, 1 );
	luaL_argcheck( L, co, 1, "coroutine expected" );
	for( std::list<LuaThread*>::iterator it = threads.begin(), end = threads.end(); it != end; ++it ){
		if( (*it)->Thread == L ){
			thread = (*it);
			break;
		}
	}
	if( thread ){
		lua_xmove( L, thread->Thread, lua_gettop( L ) - 1 );
		int status = lua_resume( thread->Thread, lua_gettop( thread->Thread ) - 1 );
		if( status != LUA_YIELD ){
			if( status > 1 ){
				string err = lua_tostring( thread->Thread, -1 );
				debug( 3, "Thread resumption failed: " + err + ".\n" );
			}
			RemoveThread( thread->ThreadId );
		}
	}else{
		debug( 3, "Thread not registred: " + getDebugInfo( L ) + ".\n" );
	}

	return 0;
}

int threadsManager::ThreadWait( lua_State* L )
{
	unsigned int tid;

	luaL_argcheck(L, lua_isnumber(L, 1), 1, "Number expected");

	if( lua_pushthread(L) ){
		luaL_where(L, 0);
		debug( 3, "Main thread is not a coroutine: " + getDebugInfo( L ) + lua_tostring(L, -1) + ".\n" );
		lua_pop(L, lua_gettop(L));
		return 0;
	}else{
		lua_pop(L, 1);
	}

	tid = GetThread(L);

	if( tid > 0 ){
		//FIXME: timer
		//AddThreadTimerEvent(lua_tointeger(L, 1), (*it)->refKey, (*it)->pausable);
	}
	lua_remove(L, 1);

	return lua_yield(L, lua_gettop(L));
}


bool threadsManager::RemoveThread( unsigned int id )
{
	LuaThread* thread;
	if( id > 0 || id <= threads.size( ) ){
		thread = NULL;
		for( std::list<LuaThread*>::iterator it = threads.begin(), end = threads.end(); it != end; ++it ){
			if( (*it)->ThreadId == id ){
				thread = (*it);
				threads.erase(it);
				break;
			}
		}
		if( thread ){
			delete thread;
			return true;
		}
	}
	return false;
}
