/*
 * CUData.cpp
 *
 *  Created on: 19.05.2012
 *
 * Origin: iiChantra
 *
 */

// This file contains the implementation details of type-to-lua binding system.
// All type specific declarations are in userdata_binding.hpp

#include <typeinfo>

#include "3rdparty/TypeList.h"

#include "3rdparty/CUData.h"
#include "3rdparty/CUDataTemplates.h"
#include "scripts/LuaScript.h"
//#include "lua_pusher.hpp"


// The block 1 declares the typelist.
#undef INCLUDE_BLOCK
#define INCLUDE_BLOCK 1
#include "scripts/CUDataBind.h"


// Array of references to userdata's metatables for each registerd class
// Those metatables are used to check types and to declare all methods of userdata
LuaRegRef udata_refs[TL::Length<ClassesList>::value] = { };
#define UD_TYPE_ID(T)  TL::IndexOf<ClassesList, T>::value
#define UD_META_REF(T) udata_refs[UD_TYPE_ID(T)]



// Function template for userdata creation.
// This function must be called from implementations of CUDataUser::createUData
template <typename T>
CUData* CreateUData(T& user)
{
	extern LuaScript* luaScript;

	lua_State* Lst = luaScript->getState();

	CUData* ud = static_cast<CUData*>(lua_newuserdata(Lst, sizeof(CUData)));
	if (!ud)
		return NULL;
	// st: ud

	CUDataUser* p = static_cast<CUDataUser*>(&user);

	ud = new(ud) CUData(p);

	luaScript->GetFromRegistry( Lst, UD_META_REF(T) );	// st: ud meta

	lua_setmetatable( Lst, -2 );					// st: ud

	ud->setRegRef( luaScript->AddToRegistry() );		// st:

	return ud;
}


// This macro is used to explicitly declare CreateUData for TYPE and
// to implement CUDataUser::createUData for this TYPE.
// This also helps to not forget the createUData declaration in TYPE.
#define UDATA_CREATOR_FUNC_DECL(TYPE)                                  \
	template CUData* CreateUData<TYPE>(TYPE& user);                    \
	CUData* TYPE::createUData()                                        \
	{                                                                  \
		return CreateUData(*this);                                     \
	}



// The block 2 uses UDATA_CREATOR_FUNC_DECL macro to declare needed functions.
#undef INCLUDE_BLOCK
#define INCLUDE_BLOCK 2
#include "scripts/CUDataBind.h"


// Used for check userdata's metatable.
// Will throw lua error if throw_error == true (default). Otherwise, will return NULL.
template <typename T>
CUData* check_userdata(lua_State* L, int ud, bool throw_error /* = true */ )
{
	CUData *p = reinterpret_cast<CUData*>( lua_touserdata( L, ud ) );
	if( p != NULL ){  /* value is a userdata? */
		if( lua_getmetatable( L, ud ) ){  /* does it have a metatable? */
			extern LuaScript* luaScript;
			luaScript->GetFromRegistry( L, UD_META_REF(T) );  /* get correct metatable */
			if( lua_rawequal( L, -1, -2 ) ){  /* does it have the correct mt? */
				lua_pop( L, 2 );  /* remove both metatables */
				return p;
			}
		}
	}
	if( throw_error )
		luaL_argerror( L, ud, "got wrong userdata" );  /* else error */
	// luaL_argerror will not return.
	return NULL;
}


// Special standard methamethods. It is wise to use them for all methatables (at least, on_gc).
// Invoked on gc event. Used to make the user object forget the CUData existance.
template <typename T>
int on_gc(lua_State* L)
{
	CUData* ud = check_userdata<T>( L, 1 );
	ud->onGC();
	return 0;
}

template <typename T>
int on_tostring(lua_State* L)
{
	CUData* ud = check_userdata<T>( L, 1 );
	lua_pushfstring( L, "userdata: %p, %s", ud, typeid(T).name() );
	return 1;
}

template <typename T>
int object_present(lua_State* L)
{
	CUData* ud = check_userdata<T>( L,  1);
	if( !ud->getUser() )
		lua_pushboolean( L, false );
	else
		lua_pushboolean( L, true );
	return 1;
}

#include "3rdparty/CUDataMacro.h"



// The block 4 is used to declare all the methods of metatables and the arrays for metatable registrations
#undef INCLUDE_BLOCK
#define INCLUDE_BLOCK 4
#include "scripts/CUDataBind.h"


// Registers metatable for type T
template <typename T>
void RegisterTypeMetatable(lua_State* L)
{
	LuaStackChecker sc( L, __FILE__, __LINE__ );
	extern LuaScript* luaScript;

	lua_newtable( L );                        // st: mt
	lua_pushstring( L, "__index" );           // st: mt __index
	lua_pushvalue( L, -2 );                   // st: mt __index mt
	lua_settable( L, -3 );     /* metatable.__index = metatable */
	// st: mt

	luaL_register( L, NULL, ud_meta[UD_TYPE_ID(T)] );
	UD_META_REF(T) = luaScript->AddToRegistry();
	// st:

}



// The block 3 is used to register all the metatables using RegisterTypeMetatable function templates
#undef INCLUDE_BLOCK
#define INCLUDE_BLOCK 3
#include "scripts/CUDataBind.h"


#undef INCLUDE_BLOCK

