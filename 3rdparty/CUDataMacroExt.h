/*
 * CUDataMacroExt.h
 *
 *  Created on: 20.05.2012
 *
 * This file not 3rdparty but placed here
 *
 * This file was created for decomposition. It included once in
 * 3rdparty/CUData.cpp and does not need to be precompiled.
 *
 */

//////////////////////////////////////////////////////////////////////////
// Macros for declaration of execution method
#define EXEC_METHOD_DECL(FIELD)											\
	template <typename T>												\
	int exec_##FIELD( lua_State* L ) {									\
		CUData* ud = check_userdata<T>( L, 1 );							\
		if( !ud->getUser( ) )											\
			luaL_error( L, "Object destroyed" );						\
		else															\
			lua_pop( L, 1 ); /* Remove userdata from stack  */			\
		return pushToLua(L, static_cast<T*>(ud->getUser())->FIELD(L));	\
	}


// Macros for getting the pointer to getter method
#define EXEC_METHOD(ID, FIELD)                          \
	&exec_##FIELD<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define EXEC_METHOD_ENTRY(ID, FIELD)    \
	{ #FIELD, EXEC_METHOD(ID, FIELD) }
//////////////////////////////////////////////////////////////////////////



// Macros for declaration of getset function method
#define GETSETF_METHOD_DECL(FIELD)                                                               \
	template <typename T>                                                                       \
	int getsetf_##FIELD( lua_State* L ){                                                         \
		CUData* ud = check_userdata<T>(L, 1);                                                   \
		if (!ud->getUser())                                                                     \
			luaL_error(L, "Object destroyed");                                                  \
		T* obj = static_cast<T*>(ud->getUser());                                                \
		bool typecheck = CHECK_INPUT_PARAM(L, 2, obj->get##FIELD());                            \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck failed");                 \
		if (typecheck)                                                                          \
			obj->set##FIELD( getFromLua< typeof(obj->get##FIELD()) >( L, 2 ) );					\
		return pushToLua(L, obj->get##FIELD() );                                                \
	}


// Macros for getting the pointer to getset method
#define GETSETF_METHOD(ID, FIELD)                          \
	&getsetf_##FIELD<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define GETSETF_METHOD_ENTRY(ID, FIELD)    \
	{ #FIELD, GETSETF_METHOD(ID, FIELD) }
//////////////////////////////////////////////////////////////////////////

