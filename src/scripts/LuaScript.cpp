/*
 * LuaScript.cpp
 *
 *  Created on: 21.09.2010
 */

#include "LuaScript.h"
#include "api.h"
#include "config.h"
#include "debug.h"
#include "safestring.h"
#include "assert.h" //WHY?

//FIXME: Архитектуропроблемы
LuaScript* luaScript = new LuaScript();


// Бездумно воровать - плохо
typedef std::map<LuaRegRef, int> RegRefCountsMap;
static RegRefCountsMap regRefCounts;


LuaScript::LuaScript( )
{
}


bool LuaScript::OpenFile( std::string name )
{
	extern MainConfig conf;
	return LuaMain::OpenFile( ( ( conf.path.scripts != "" ) ? conf.path.scripts : "data/scripts/" ) + name + ".lua" );
}


//////////////////////////////////////////////////////////////////////////
// Заносит в реестр то, что лежит на вершине стека, и возвращает ссылку на него
// При этом с вершины стека оно удаляется
LuaRegRef LuaScript::AddToRegistry( )
{
	// Стек: ... data
	return luaL_ref( Lst, LUA_REGISTRYINDEX );	// Стек:
}

// Удаляет из реестра содержимое по ссылке
void LuaScript::RemoveFromRegistry( LuaRegRef& r )
{
	lua_unref( Lst, r );
	r = LUA_NOREF;
}

// Помещает на верхушку стека значенее из реестра
void LuaScript::GetFromRegistry( lua_State* L, LuaRegRef r )
{
	lua_rawgeti( Lst, LUA_REGISTRYINDEX, r );
	lua_xmove( Lst, L, 1 );
}


// Помещает в procref ссылку реестра луа на функцию, находящуюся в стеке под номером n
void LuaScript::RegProc( lua_State* L, LuaRegRef* procref, int n )
{
	luaL_argcheck(L, lua_isfunction(L, n) || lua_isnil(L, n), n, "Function or nil expected");

	RegObject( L, procref, n );
}

// Помещает в procref ссылку реестра луа на объект, находящийся в стеке под номером n
void LuaScript::RegObject( lua_State* L, LuaRegRef* procref, int n )
{
	ReleaseObject(procref);

	if(!lua_isnil(L, n))
	{
		// TODO: сделать перемещение функции на самый верх стека и забирать ее оттуда
		// Сейчас же просто забирается одно верхнее значение со стека, это может быть и не функция
		lua_xmove( L, Lst, 1 );
		*procref = AddToRegistry();
		ReserveObject(*procref);
	}
}

// Увеличивает счетчик количества использований ссылки на реестр
void LuaScript::ReserveObject( LuaRegRef procref )
{
	if( procref == LUA_NOREF )
		return;
	RegRefCountsMap::iterator it = regRefCounts.find(procref);
	if( it == regRefCounts.end() ){
		regRefCounts[procref] = 1;
	}else{
		assert( it->second >= 0 );
		++(it->second);
	}
}

// Уменьшает счетчик количества использований ссылки на реестр.
// Если счетчик достигает 0, объект из реестра удаляется.
void LuaScript::ReleaseObject( LuaRegRef* procref )
{
	LuaStackChecker sc( Lst, __FILE__, __LINE__ );

	if( *procref == LUA_NOREF )
		return;

	RegRefCountsMap::iterator it = regRefCounts.find(*procref);
	if( it == regRefCounts.end() )
		return;

	assert(it->second > 0);
	if(it->second <= 0){
		Debug::debug( Debug::SCRIPT, "LuaScript::ReleaseObject: procref " +
				citoa(*procref) + " references count is " + citoa(it->second) + ".\n" );
		it->second = 1;
	}

	--(it->second);
	if(it->second == 0){
		RemoveFromRegistry(*procref);
		*procref = LUA_NOREF;
		regRefCounts.erase(it);
	}
}


int LuaScript::ExecChunk( UINT args )
{
	int level = lua_gettop( Lst )  - args - 1; //Верх стека - аргументы - одна функция
	if( lua_pcall( Lst, args, LUA_MULTRET, 0 ) ){
		std::string err = lua_tostring( Lst, -1 );
		Debug::debug( Debug::SCRIPT, "In ExecChunk(): " + err + ".\n" );
		return -1;
	}
	return lua_gettop( Lst ) - level; //Новый верх стека - старый = количество результатов
}

int LuaScript::ExecChunkFromReg( LuaRegRef r, UINT args )
{
	GetFromRegistry( Lst, r );

	int i = lua_gettop( Lst ) - args;
	if (i <= 0)
		i = 1;

	lua_insert( Lst, i );

	return ExecChunk( args );
}

std::string to_string( lua_State* L, int idx )
{
	std::string s = "";
	char buf[20];
	if( idx < 0 ){
		idx += lua_gettop( L ) + 1;
	}

	int t = lua_type( L, idx );
	switch( t ){
		case LUA_TSTRING: /* strings */
			s = (std::string)"'" + lua_tostring( L, idx ) + "'";
			break;

		case LUA_TBOOLEAN: /* booleans */
			s = lua_toboolean( L, idx ) ? "true" : "false";
			break;

		case LUA_TNUMBER: /* numbers */
			snprintf(buf, 20, "%f", lua_tonumber( L, idx ));
			s = (std::string)buf;
			break;

		case LUA_TTHREAD:
			snprintf(buf, 20, "0x%p", (void*)lua_tothread( L, idx ));
			s = "thread " + (std::string)buf;
			break;

		case LUA_TTABLE:
			s = "table: {";
			lua_pushnil( L );  /* first key */
			while( lua_next( L, idx ) != 0 ){
				/* `key' is at index -2 and `value' at index -1 */
				s += to_string( L, -2 ) + ": ";
				s += to_string( L, -1 ) + ", ";
				lua_pop(L, 1);  /* removes `value'; keeps `key' for next iteration */
			}
			s += "}";
			break;

		default: /* other values */
			s += lua_typename( L, t );
			break;

	}

	return s;
}


// Выводит в лог дамп содержимого стека.
void LuaScript::StackDumpToLog( lua_State *L )
{
	int i;
	int top = lua_gettop(L);
	std::string s = "";
	char buf[20];
	snprintf(buf, 20, "%p: ", (void*)L);

	s += buf;

	for (i = 1; i <= top; i++){
		/* repeat for each level */
		s += to_string( L, i ) + ", ";
	}
	s+= "\n";

	Debug::debug( Debug::SCRIPT, s.c_str() );
}


