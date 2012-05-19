/*
 * CUDataBind.h
 *
 *  Created on: 19.05.2012
 *
 */

//////////////////////////////////////////////////////////////////////////
//
// How to add new class?
// 1. Put name of class in typelist ClassesList (the order of classes is important, they enumerated from 0).
//    Class must implement createUData virtual function from CUDataUser class
// 2. Declare the function for userdata creation (use UDATA_CREATOR_FUNC_DECL macro)
// 3. Add RegisterTypeMetatable call for new class in RegisterAllTypeMetatables function.
//
//////////////////////////////////////////////////////////////////////////
//
// 4-a. How to add new getter for field?
//    Add GETTER_METHOD_DECL(field_name)
//    Add GETTER_METHOD_ENTRY(number, field_name) in ud_meta array, where
//      number is number of class in ClassesList typelist;
//      field_name is name of public field in this class
// 4-b. How to add new getter/setter for field?
//    Add GETSET_METHOD_DECL(field_name)
//    Add GETSET_METHOD_ENTRY(number, field_name) in ud_meta array, where
//      number is number of class in ClassesList typelist;
//      field_name is name of public field in this class
//
// There are more declaration macros like GETSET_METHOD_DECL in 3rdparty/CUData.cpp.
//
// If there is field with equeal names in different classes, only one declaration must be.
// METAMETHODS_MAX_COUNT value can be increased, if needed.
//
//////////////////////////////////////////////////////////////////////////
//
// Structure of this file:
// This file consists of several include blocks. This helps to separate implementation details
// from declarations and to simplify the maintance of the code.
// This file is included by the udata.cpp only.


#if INCLUDE_BLOCK == 1

// Includes of declarations of all types in typelist
#include "units/Unit.h"
#include "units/Animated.h"
#include "units/Dynamic.h"
#include "units/Entity.h"
#include "units/Plant.h"
#include "units/Corpse.h"
#include "units/Player.h"

// Typelist with all types, that can be passed to lua as userdata
typedef TYPELIST_7(Unit, AnimatedUnit, DynamicUnit, Entity, Plant, Corpse, Player) ClassesList;



#elif INCLUDE_BLOCK == 2

// 2. For each type, it's needed to explicitly declare functions for userdata creation.
UDATA_CREATOR_FUNC_DECL(Unit);
UDATA_CREATOR_FUNC_DECL(AnimatedUnit);
UDATA_CREATOR_FUNC_DECL(DynamicUnit);
UDATA_CREATOR_FUNC_DECL(Entity);
UDATA_CREATOR_FUNC_DECL(Plant);
UDATA_CREATOR_FUNC_DECL(Corpse);
UDATA_CREATOR_FUNC_DECL(Player);



#elif INCLUDE_BLOCK == 3

// 3. Registration of metatables
// This functions will be called on lua initialization in SCRIPT::InitScript
void RegisterAllTypeMetatables(lua_State* L)
{
	// Please, don't change this line
	std::fill(udata_refs, udata_refs + TL::Length<ClassesList>::value, LUA_NOREF);

	// Make RegisterTypeMetatable calls for each type
	RegisterTypeMetatable<Unit>(L);
	RegisterTypeMetatable<AnimatedUnit>(L);
	RegisterTypeMetatable<DynamicUnit>(L);
	RegisterTypeMetatable<Entity>(L);
	RegisterTypeMetatable<Plant>(L);
	RegisterTypeMetatable<Corpse>(L);
	RegisterTypeMetatable<Player>(L);

}


#elif INCLUDE_BLOCK == 4

// 4. Declaration of methods in metatables


// Meta methods
const size_t METAMETHODS_MAX_COUNT = 70;
static const struct luaL_reg ud_meta[TL::Length<ClassesList>::value][METAMETHODS_MAX_COUNT] = {
	{
		STD_METHODS(0),
		//GAMEOBJ_METH_ENTRY(0),
		END
	},
	{
		STD_METHODS(1),
		//OBJPHYS_METH_ENTRY(1),
		END
	},
	{
		STD_METHODS(2),
		//OBJDYNAMIC_METH_ENTRY(2),
		END
	},
	{
		STD_METHODS(3),
		//OBJPLAYER_METH_ENTRY(3),
		END
	},
	{
		STD_METHODS(4),
		//OBJENEMY_METH_ENTRY(4),
		END
	},
	{
		STD_METHODS(5),
		//OBJBULLET_METH_ENTRY(5),
		END
	},
	{
		STD_METHODS(6),
		//OBJRAY_METH_ENTRY(6),
		END
	}
	//{
	//	STD_METHODS(7),
	//	//OBJENV_METH_ENTRY(7),
	//	END
	//}
};



#endif
