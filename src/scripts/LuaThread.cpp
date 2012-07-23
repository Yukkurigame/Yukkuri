/*
 * LuaThread.cpp
 *
 *  Created on: 31.10.2010
 */

#include "LuaThread.h"
#include "LuaScript.h"
#include "3rdparty/timer/TimerManager.h"
#include <list>
#include <cstdlib>

#include "hacks.h"
#include "safestring.h"

#include "debug.h"
using namespace Debug;

extern LuaScript* luaScript;


static int threadsCounter = 0;

static std::list<LuaThread*> threads;

static std::string getDebugInfo( lua_State* L )
{
	lua_Debug ar;
	//FIXME: it's cruve.
	char d[65];
	lua_getinfo(L, ">S", &ar);
	snprintf(d, 65, "%s line %d", ar.short_src, ar.linedefined );
	return (std::string)d;
}

LuaThread::LuaThread( )
{
	ThreadId = ++threadsCounter;
	Thread = NULL;
	refKey = LUA_NOREF;
	Pausable = false;
}

LuaThread::LuaThread( lua_State* self )
{
	ThreadId = ++threadsCounter;
	Thread = self;
	refKey = LUA_NOREF;
	Pausable = false;
}

LuaRegRef threadsManager::NewThread( lua_State* L )
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
		pausable = lua_toboolean(L, 2) != 0;
		lua_pop(L, 1);
	}

	if( !lua_isfunction( L, 1 ) || lua_iscfunction( L, 1 ) ){
		debug( SCRIPT, "Wrong first argument passed:" + getDebugInfo( L ) + ". Function expected.\n" );
		return 0;
	}

	// Код из функции luaB_cocreate файла lbaselib.c
	// Стек L: func
	NL = lua_newthread(L);	// Стек L: func thread
							// Стек NL:

	lua_pushvalue( L, 1 );	// Стек L: func thread func
	lua_xmove( L, NL, 1 );	/* move function from L to NL */

	// Конец кода из функции luaB_cocreate файла lbaselib.c

	// Стек L: func thread
	// Стек NL: func

	thread = new LuaThread( NL );

	lua_pushvalue( L, -1 );						// Стек L: func thread thread
	// А если L == Lst, то переноса не будет, с вершины стека просто снимется 1 копия thread
	lua_xmove( L, LuaScript::getState(), 1 );	// Стек L: func thread
												// Стек lua: thread
	thread->refKey = luaScript->AddToRegistry();	// Стек lua:
	thread->Pausable = pausable;

	threads.push_back( thread );

	char dbg[75];
	snprintf( dbg, 75, "Lua thread 0x%p created, master_thread: 0x%p, id: %d.\n", NL, L, thread->ThreadId);
	Debug::debug( Debug::SCRIPT, dbg);

	return thread->refKey;
}


ThreadIter threadsManager::GetThread( lua_State* L )
{
	FOREACHIT( threads ){
		LuaThread* lt = *it;
		if( lt->Thread == L )
			return it;
	}
	return threads.end();
}

ThreadIter threadsManager::End( )
{
	return threads.end();
}


// Вызывается при настплении события таймера, вызывает продолжение работы треда
void threadsManager::ProcessThread( LuaRegRef r )
{
	LuaThread* lt = NULL;
	ThreadIter nit = threads.end();
	FOREACHIT( threads ){
		lt = *it;
		if( lt->refKey == r ){
			nit = it;
			break;
		}
	}

	if( lt ){
		ResumeThread( nit, NULL );
	}
}


// Продолжает выполнение приостановленной coroutine, являющейся
// тредом, за котором следит игра.
// В ОТЛИЧИЕ от coroutine.resume, ничего не возвращает.
int threadsManager::ResumeThread( ThreadIter it, lua_State* L )
{
	if( it == threads.end() )
		return 1;

	LuaThread* lt = *it;

	if( L ){
		// Стек L:		thread arg1 arg2 ... argN
		// Cтек thread: [func]
		// func на стеке только в первый раз
		lua_xmove( L, lt->Thread, lua_gettop(L) - 1 );
		// Стек L:		thread
		// Cтек thread: [func] arg1 arg2 ... argN
	}


	//SCRIPT::StackDumpToLog(lt->masterState);
	//SCRIPT::StackDumpToLog(lt->lThread);
	lua_resume( lt->Thread, lua_gettop( lt->Thread ) - 1 );

	if( lua_status( lt->Thread ) != LUA_YIELD ){
		if( lua_status( lt->Thread ) != 0 ){		// Errors
			std::string err = lua_tostring( lt->Thread, -1 );
			debug( SCRIPT, "Thread resumption failed: " + err + ".\n" );
		}
		RemoveThread( it );
	}

	return 0;
}


int threadsManager::ThreadWait( lua_State* L )
{
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

	ThreadIter it = GetThread(L);
	if( it != threads.end() ){
		Timer::AddThreadTimerEvent( lua_tointeger(L, 1), (*it)->refKey, (*it)->Pausable );
	}
	lua_remove(L, 1);

	return lua_yield(L, lua_gettop(L));
}


void threadsManager::RemoveThread( ThreadIter it )
{
	LuaThread* thread = *it;
	if( thread != NULL ){
		Timer::DeleteTimerEvent( thread->refKey );
		luaScript->RemoveFromRegistry( thread->refKey );
		Debug::debug( Debug::SCRIPT, "Lua thread " + citoa(thread->ThreadId) + " removed.\n");
		delete thread, thread = NULL;
	}
	threads.erase(it);
}


void threadsManager::CleanThreads()
{
	Debug::debug( Debug::MAIN, "Cleaning threads.\n" );
	clear_vector( &threads );
}

