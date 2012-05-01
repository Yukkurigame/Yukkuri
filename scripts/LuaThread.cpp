/*
 * LuaThread.cpp
 *
 *  Created on: 31.10.2010
 */

#include "LuaThread.h"
#include <list>
#include <cstdlib>

#include "hacks.h"

#include "debug.h"
using namespace Debug;


static int threadsCounter = 0;

static std::list<LuaThread*> threads;

static std::string getDebugInfo( lua_State* L )
{
	lua_Debug ar;
	//FIXME: it's cruve.
	char d[65];
	lua_getinfo(L, ">S", &ar);
	sprintf(d, "%s line %d", ar.short_src, ar.linedefined );
	return (std::string)d;
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
		debug( SCRIPT, "Wrong arguments count passed:" + getDebugInfo( L ) + ".\n");
		return 0;
	}

	pausable = false;
	if( lua_gettop(L) == 2 ){
		pausable = lua_toboolean(L, 2);
		lua_pop(L, 1);
	}

	if( !lua_isfunction( L, 1 ) || lua_iscfunction( L, 1 ) ){
		debug( SCRIPT, "Wrong first argument passed:" + getDebugInfo( L ) + ". Function expected.\n" );
		return 0;
	}

	NL = lua_newthread(L);

	lua_pushvalue(L, 1);
	lua_xmove(L, NL, 1);

	thread = new LuaThread( NL );

	thread->Pausable = pausable;

	threads.push_back( thread );

	char dbg[75];
	sprintf( dbg, "Lua thread 0x%p created, master_thread: 0x%p, id: %d.\n", NL, L, thread->ThreadId);
	Debug::debug( Debug::SCRIPT, dbg);

	return thread->ThreadId;
}

unsigned int threadsManager::GetThread( lua_State* L )
{
	FOREACHIT( threads ){
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
	FOREACHIT( threads ){
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
				std::string err = lua_tostring( thread->Thread, -1 );
				debug( SCRIPT, "Thread resumption failed: " + err + ".\n" );
			}
			RemoveThread( thread->ThreadId );
		}
	}else{
		debug( SCRIPT, "Thread not registred: " + getDebugInfo( L ) + ".\n" );
	}

	return 0;
}

int threadsManager::ThreadWait( lua_State* L )
{
	unsigned int tid;

	luaL_argcheck(L, lua_isnumber(L, 1), 1, "Number expected");

	if( lua_pushthread(L) ){
		luaL_where(L, 0);
		debug( SCRIPT, "Main thread is not a coroutine: " + getDebugInfo( L ) +
				lua_tostring(L, -1) + ".\n" );
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
	char dbg[50];

	if( id > 0 || id <= threads.size( ) ){
		thread = NULL;
		FOREACHIT( threads ){
			if( (*it)->ThreadId == id ){
				thread = (*it);
				threads.erase(it);
				break;
			}
		}
		if( thread ){
			delete thread;
			sprintf( dbg, "Lua thread %d removed.\n", id );
			Debug::debug( Debug::SCRIPT, dbg);
			return true;
		}
	}

	sprintf( dbg, "Lua thread %d not found.\n", id );
	Debug::debug( Debug::SCRIPT, dbg);

	return false;
}

void threadsManager::CleanThreads()
{
	Debug::debug( Debug::MAIN, "Cleaning threads.\n" );
	clear_vector( &threads );
}


