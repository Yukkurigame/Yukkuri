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

// Бездумно воровать - плохо
typedef std::map<LuaRegRef, int> RegRefCountsMap;
static RegRefCountsMap regRefCounts;


LuaScript::LuaScript( )
{

}

bool LuaScript::Init()
{
	RegisterApi( );
	return true;
}

void LuaScript::RegisterApi( )
{
	lua_register( Lst, "readdir", &scriptApi::ReadDirectory );
	lua_register( Lst, "getcwd", &scriptApi::GetCWD );

	lua_register( Lst, "Debug", &scriptApi::Debug );

	lua_register( Lst, "SetBindings", &scriptApi::SetBindings );

	lua_register( Lst, "CreateWidget", &scriptApi::CreateWidget );
	lua_register( Lst, "LoadWidget", &scriptApi::LoadWidget );
	lua_register( Lst, "BindWidget", &scriptApi::BindWidget );
	lua_register( Lst, "ToggleWidget", &scriptApi::ToggleWidget );
	lua_register( Lst, "BindWidgetMaxBar", &scriptApi::BindWidgetMaxBar );
	lua_register( Lst, "WidgetChildren", &scriptApi::WidgetChildren );
	lua_register( Lst, "GetWidgetName", &scriptApi::GetWidgetName );
	lua_register( Lst, "WidgetSetBarSize", &scriptApi::WidgetSetBarSize );

	lua_register( Lst, "CreateUnit", &scriptApi::CreateUnit );
	lua_register( Lst, "DeleteUnit", &scriptApi::DeleteUnit );

	lua_register( Lst, "NewThread", &scriptApi::NewThread );
	lua_register( Lst, "ThreadWait", &scriptApi::ThreadWait );
	lua_register( Lst, "ResumeThread", &scriptApi::ResumeThread );
	lua_register( Lst, "RemoveThread", &scriptApi::RemoveThread );

	lua_register( Lst, "SetCameraTarget", &scriptApi::SetCameraTarget );

	lua_register( Lst, "LoadMapRegion", &scriptApi::LoadMapRegion );

}

bool LuaScript::OpenFile( std::string name )
{
	extern MainConfig conf;
	return LuaMain::OpenFile( ( ( conf.scriptsPath != "" ) ? conf.scriptsPath : "data/scripts/" ) + name + ".lua" );
}


//////////////////////////////////////////////////////////////////////////
// Заносит в реестр то, что лежит на вершине стека, и возвращает ссылку на него
// При этом с вершины стека оно удаляется
LuaRegRef LuaScript::AddToRegistry( )
{
	// Стек: ... data
	return luaL_ref(Lst, LUA_REGISTRYINDEX);	// Стек:
}

// Удаляет из реестра содержимое по ссылке
void LuaScript::RemoveFromRegistry( LuaRegRef& r )
{
	lua_unref(Lst, r);
	r = LUA_NOREF;
}

// Помещает на верхушку стека значенее из реестра
void LuaScript::GetFromRegistry( lua_State* L, LuaRegRef r )
{
	lua_rawgeti(Lst, LUA_REGISTRYINDEX, r);
	lua_xmove(Lst, L, 1);
}


// Помещает в procref ссылку реестра луа на функцию, находящуюся в стеке под номером n
void LuaScript::RegProc( lua_State* L, LuaRegRef* procref, int n )
{
	luaL_argcheck(L, lua_isfunction(L, n) || lua_isnil(L, n), n, "Function or nil expected");

	ReleaseProc(procref);

	if(!lua_isnil(L, n))
	{
		// TODO: сделать перемещение функции на самый верх стека и забирать ее оттуда
		// Сейчас же просто забирается одно верхнее значение со стека, это может быть и не функция
		lua_xmove(L, Lst, 1);
		*procref = AddToRegistry();
		ReserveProc(*procref);
	}
}

// Увеличивает счетчик количества использований ссылки на реестр
void LuaScript::ReserveProc( LuaRegRef procref )
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
void LuaScript::ReleaseProc( LuaRegRef* procref )
{
	LuaStackChecker sc(Lst, __FILE__, __LINE__);

	if( *procref == LUA_NOREF )
		return;

	RegRefCountsMap::iterator it = regRefCounts.find(*procref);
	if( it == regRefCounts.end() )
		return;

	assert(it->second > 0);
	if(it->second <= 0){
		char dbg[100];
		snprintf(dbg, 99, "LuaScript::ReleaseProc: procref %d references count is %d.\n", *procref, it->second);
		Debug::debug( Debug::SCRIPT, dbg );
		it->second = 1;
	}

	--(it->second);
	if(it->second == 0){
		RemoveFromRegistry(*procref);
		*procref = LUA_NOREF;
		regRefCounts.erase(it);
	}
}


int LuaScript::ExecChunk( )
{
	int level = lua_gettop( Lst ) - 1; // - args //Верх стека - аргументы - одна функция
	if( lua_pcall( Lst, 0, LUA_MULTRET, 0 ) ){ // args
		std::string err = lua_tostring( Lst, -1 );
		Debug::debug( Debug::SCRIPT, "In ExecChunk(): " + err );
		return -1;
	}
	return lua_gettop( Lst ) - level; //Новый верх стека - старый = количество результатов
}

int LuaScript::ExecChunkFromReg( LuaRegRef r )
{
	GetFromRegistry( Lst, r );

	//TODO: args
	int i = lua_gettop( Lst ); // - args;
	if (i <= 0)
		i = 1;

	lua_insert( Lst, i );

	return ExecChunk( ); // (args);
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
		int t = lua_type(L, i);
		switch (t)
		{
		case LUA_TSTRING: /* strings */
			s += (std::string)"'" + lua_tostring(L, i) + "'";
			break;

		case LUA_TBOOLEAN: /* booleans */
			s += lua_toboolean(L, i) ? "true" : "false";
			break;

		case LUA_TNUMBER: /* numbers */
			snprintf(buf, 20, "%f", lua_tonumber(L, i));
			s += (std::string)buf;
			break;

		case LUA_TTHREAD:
			snprintf(buf, 20, "0x%p", (void*)lua_tothread(L, i));
			s += "thread " + (std::string)buf;
			break;

		default: /* other values */
			s += lua_typename(L, t);
			break;

		}
		s += ", "; /* put a separator */
	}
	s+= "\n";

	Debug::debug( Debug::CONFIG, s.c_str() );
}


