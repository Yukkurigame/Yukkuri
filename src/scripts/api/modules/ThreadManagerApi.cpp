/*
 * ThreadManager.cpp
 *
 *  Created on: 22.05.2012
 *
 */

#include "scripts/api/modules/ThreadManagerApi.h"
#include "scripts/LuaThread.h"

#include "3rdparty/timer/TimerManager.h"



int ThreadApi::newThread(lua_State* L)
{
	luaL_argcheck(L, lua_isfunction(L, 1), 1, "function expected");
	luaL_argcheck(L, lua_isboolean(L, 2) || lua_isnoneornil(L, 2), 2, "boolean or none expected");
	// Стек L: func pausable
	threadsManager::NewThread( L );
	// Стек L: func thread
	return 1;
}


// Удаляет тред. Биндинг для луа.
// Возвращает true или false.
int ThreadApi::stopThread( lua_State* L )
{
	luaL_argcheck( L, lua_isthread( L, 1 ), 1, "Thread expected" );
	lua_State* t = lua_tothread( L, 1 );
	ThreadIter it = threadsManager::GetThread( t );
	if( it != threadsManager::End() ){
		threadsManager::RemoveThread( it );
		lua_pushboolean( L, 1 );
		return 1;
	}else{
		lua_pushboolean( L, 0 );
		lua_pushstring( L, "Thread is not registered" );
		return 2;
	}
}


// Удаляет все треды. Биндинг для луа.
int ThreadApi::stopAllThreads( lua_State* L )
{
	threadsManager::CleanThreads();
	return 0;
}


// Функция приостанавливает coroutine, из которой вызвана. Аналогична
// команде базовой библиотеки lua coroutines.yield, но требует первый
// аргумент - число милисекунд, на которое coroutine будет приостановлена.
// Однако, передавать параметры для возврата бессмысленно, resume ничего не вернет.
int ThreadApi::wait( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Number expected" );

	if( lua_pushthread( L ) ){
		// lua_pushthread вернет true, если мы пытаемся остановить основной поток
		luaL_where( L, 0 );
		Debug::debug( Debug::SCRIPT, std::string(lua_tostring(L, -1)) + " attempting to Wait on main thread.\n");
		lua_pop(L, lua_gettop(L));
		return 0;
	}else{
		// lua_pushthread вернула false, мы не в основном потоке, убираем из стека ненужное значение
		lua_pop(L, 1);
	}

	ThreadIter it = threadsManager::GetThread( L );
	if( it != threadsManager::End() ){
		Timer::AddThreadTimerEvent( static_cast<UINT>(lua_tointeger(L, 1)), (*it)->refKey, (*it)->Pausable );
	}
	lua_remove(L, 1);

	return lua_yield(L, lua_gettop(L));
}


// Продолжает выполнение приостановленной coroutine.
// Аналогична команде coroutines.resume, однако работает
// несколько дольше, так как сначала ищет среди созданных игрой тредов.
// Не возвращает аргументы, преданные через Wait
int ThreadApi::resume(lua_State* L)
{
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	ThreadIter it = threadsManager::GetThread(co);
	if( it != threadsManager::End() ){
		return threadsManager::ResumeThread( it, L );
	}

	// Куски luaB_coresume (coroutine.resume в lua) и auxresume из недр lua 5.1.
	// Это необходмио, чтобы повторить поведение coroutine.resume.
	// TODO: возможно, проще и правильнее вызвать coroutine.resume через lua_pcall, но мне
	// сейчас легче было скопипастить куски кода из библиотеки, чем вспоминать, как им пользоваться.
	// Да и вообще, код сюда дойдет, только если кто-то догадается использовать этот Resume вместо
	// coroutine.resume для продолжения треда, созданного не с помощью NewThread.
	int narg = lua_gettop(L) - 1;
	lua_xmove(L, co, narg);
	int status = lua_resume(co, narg);
	int r;
	if (status == 0 || status == LUA_YIELD) {
		int nres = lua_gettop(co);
		if (!lua_checkstack(L, nres + 1))
			luaL_error(L, "too many results to resume");
		lua_xmove(co, L, nres);  /* move yielded values */
		//return nres;
		r = nres;
	}
	else {
		lua_xmove(co, L, 1);  /* move error message */
		//return -1;  /* error flag */
		r = -1;
	}
	if (r < 0) {
		lua_pushboolean(L, 0);
		lua_insert(L, -2);
		return 2;  /* return false + error message */
	}
	else {
		lua_pushboolean(L, 1);
		lua_insert(L, -(r + 1));
		return r + 1;  /* return true + `resume' returns */
	}
}

