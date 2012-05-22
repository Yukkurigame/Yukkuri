/*
 * LuaBase.cpp
 *
 *  Created on: 30.04.2012
 *
 */

#include "Lua.h"

#include "3rdparty/CUDataTemplates.h"

#include "api.h"
#include "api/UnitManager.h"
#include "api/Interface.h"
#include "api/ThreadManager.h"

#include "Bindings.h"
#include "units/Prototypes.h"
#include "units/Unit.h"



#include "debug.h"


void LuaMain::RegisterApi( lua_State* L )
{
	// Register UData metatables
	RegisterAllTypeMetatables( L );


	// Register base methods
	lua_register( L, "readdir", &scriptApi::ReadDirectory );
	lua_register( L, "getcwd", &scriptApi::GetCWD );

	lua_register( L, "Debug", &scriptApi::Debug );

	lua_register( L, "SetBindings", &scriptApi::SetBindings );

	lua_register( L, "DeleteUnit", &scriptApi::DeleteUnit );

	lua_register( L, "SetCameraTarget", &scriptApi::SetCameraTarget );

	lua_register( L, "LoadMapRegion", &scriptApi::LoadMapRegion );



	// Libraries
	luaL_register( L, "UnitManager", UMApi::methods );
	luaL_register( L, "Interface", IfaceApi::methods );
	luaL_register( L, "Thread", ThreadApi::methods );


	// Регистрируем константные названия для клавиш.
	lua_newtable( L );			// Создаем новую таблицу. Стек: таблица
	{
		for( UINT i = 1; i < MAXKEYS; i++){		// Цикл по всем возможным кодам
			lua_pushinteger( L, i );		// Помещаем в стек код клавиши. Стек: таблица, i
			lua_setfield( L, -2, Bindings::bnd.KeyName(i).c_str() ); // Заносим в таблицу перменную с именем и значением. Стек: таблица
		}
	}
	lua_setglobal(L, "keys");		// Заносим таблицу в глобальную переменную. Стек:


	lua_newtable(L);			// Создаем новую таблицу. Стек: таблица
	{
		for( int i = 0; i < Binds::bndLast; i++ ){		// Цикл по всем возможным функциям в конфиге
			lua_pushinteger( L, i );
			lua_setfield( L, -2, Binds::getFunctionName( (Binds::func_numbers)i ) );
		}
	}
	lua_setglobal(L, "bindings");		// Заносим таблицу в глобальную переменную. Стек:


#define FIELD(name) \
	lua_pushinteger( L, name ); lua_setfield( L, -2, #name );

	// Различные именованые константы
	lua_newtable(L);		// Стек: таблица
	{
		// Unit Types
		FIELD(utStatic)
		FIELD(utPlayer)
		FIELD(utEntity)
		FIELD(utPlant)
		FIELD(utCorpse)


		// ***********************
		// Actions
		FIELD(acNone)

		// Functions
		FIELD(acSuper)

		// Action parameters stack
		FIELD(acPushInt)
		FIELD(acPushFloat)
		FIELD(acPushString)

		// Unit Parameters
		FIELD(acSetParam)
		FIELD(acCopyParam)
		FIELD(acLoadParam)
		FIELD(acLoadParamBunch)

	}
	lua_setglobal(L, "constants");		// Заносим таблицу в глобальную переменную. Стек:

#undef FIELD

}

void LuaMain::RegisterLib( std::string name, const luaL_Reg* functions )
{
	luaL_register( Lst, name.c_str(), functions );
}
