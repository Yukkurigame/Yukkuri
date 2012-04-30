/*
 * LuaBase.cpp
 *
 *  Created on: 30.04.2012
 *
 */

#include "Lua.h"

#include "api.h"

#include "Bindings.h"
#include "units/Prototypes.h"

#include "debug.h"


void LuaMain::RegisterApi( lua_State* L )
{
	lua_register( L, "readdir", &scriptApi::ReadDirectory );
	lua_register( L, "getcwd", &scriptApi::GetCWD );

	lua_register( L, "Debug", &scriptApi::Debug );

	lua_register( L, "SetBindings", &scriptApi::SetBindings );

	lua_register( L, "CreateWidget", &scriptApi::CreateWidget );
	lua_register( L, "LoadWidget", &scriptApi::LoadWidget );
	lua_register( L, "BindWidget", &scriptApi::BindWidget );
	lua_register( L, "ToggleWidget", &scriptApi::ToggleWidget );
	lua_register( L, "BindWidgetMaxBar", &scriptApi::BindWidgetMaxBar );
	lua_register( L, "WidgetChildren", &scriptApi::WidgetChildren );
	lua_register( L, "GetWidgetName", &scriptApi::GetWidgetName );
	lua_register( L, "WidgetSetBarSize", &scriptApi::WidgetSetBarSize );

	lua_register( L, "CreateUnit", &scriptApi::CreateUnit );
	lua_register( L, "DeleteUnit", &scriptApi::DeleteUnit );

	lua_register( L, "NewThread", &scriptApi::NewThread );
	lua_register( L, "ThreadWait", &scriptApi::ThreadWait );
	lua_register( L, "ResumeThread", &scriptApi::ResumeThread );
	lua_register( L, "RemoveThread", &scriptApi::RemoveThread );

	lua_register( L, "SetCameraTarget", &scriptApi::SetCameraTarget );

	lua_register( L, "LoadMapRegion", &scriptApi::LoadMapRegion );



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

		// Actions
		FIELD(acNone)

	}
	lua_setglobal(L, "constants");		// Заносим таблицу в глобальную переменную. Стек:

#undef FIELD

}

