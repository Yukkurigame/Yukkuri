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
#include "api/CameraApi.h"
#include "api/BindingsApi.h"

#include "Bindings.h"
#include "units/Prototypes.h"
#include "units/Unit.h"
#include "units/YOBA.h"



#include "debug.h"


void LuaMain::RegisterApi( lua_State* L )
{
	// Register UData metatables
	RegisterAllTypeMetatables( L );


	// Register base methods
	lua_register( L, "readdir", &scriptApi::ReadDirectory );
	lua_register( L, "getcwd", &scriptApi::GetCWD );

	lua_register( L, "Debug", &scriptApi::Debug );

	lua_register( L, "LoadMapRegion", &scriptApi::LoadMapRegion );



	// Libraries
	luaL_register( L, "UnitManager", UMApi::methods );
	luaL_register( L, "Interface", IfaceApi::methods );
	luaL_register( L, "Thread", ThreadApi::methods );
	luaL_register( L, "Camera", CameraApi::methods );
	luaL_register( L, "Input", BindingsApi::methods );


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

		// Action parameters stack
		FIELD(acPush)
		FIELD(acPushInt)
		FIELD(acPushFloat)
		FIELD(acPushString)

		// Functions
		FIELD(acSuper)
		FIELD(acRestoreState)
		FIELD(acRepeatDelay)
		FIELD(acSetAction)


		// Conditions
		FIELD(acCondition)
		FIELD(acIfParamEqual)
		FIELD(acIfParamLess)
		FIELD(acIfParamMore)
		FIELD(acIfParametersEqual)
		FIELD(acIfParametersLess)
		FIELD(acIfParametersMore)
		FIELD(acIfParametersLessBy)
		FIELD(acIfFlag)
		FIELD(acIfNotFlag)
		FIELD(acEnd)

		// Unit flags
		FIELD(ufDeleted)
		FIELD(ufEdible)
		FIELD(ufMoving)
		FIELD(ufLast)
		FIELD(acSetFlag)
		FIELD(acRemoveFlag)

		// Unit Parameters
		FIELD(acSetParam)
		FIELD(acCopyParam)
		FIELD(acLoadParam)
		FIELD(acLoadParamBunch)

		// Misc commands
		FIELD(acMove)
		FIELD(acSetUnitSize)
		FIELD(acEAddPathTarget)

		// Unit base parameters
		FIELD(uBaseLevel)
		FIELD(uBaseAge)
		FIELD(uBaseKills)
		// Unit attributes
		FIELD(uAttrStrength)
		FIELD(uAttrIntelligence)
		FIELD(uAttrAgility)
		FIELD(uAttrLuck)
		// Unit characteristics
		FIELD(uChrSpeed)
		FIELD(uChrDamage)
		FIELD(uChrDefence)
		FIELD(uChrNutritive)
		// Unit parametes
		FIELD(uParamHP)
		FIELD(uParamExp)
		FIELD(uParamFed)
		// Unit state
		FIELD(uStateHP)
		FIELD(uStateExp)
		FIELD(uStateFed)

	}
	lua_setglobal(L, "constants");		// Заносим таблицу в глобальную переменную. Стек:

#undef FIELD

}

void LuaMain::RegisterLib( std::string name, const luaL_Reg* functions )
{
	luaL_register( Lst, name.c_str(), functions );
}
