/*
 * CUDataMacro.h
 *
 *  Created on: 19.05.2012
 *
 * Origin: iiChantra
 *
 *
 * This file was created for decomposition. It included once in
 * 3rdparty/CUData.cpp and does not need to be precompiled.
 *
 */



//////////////////////////////////////////////////////////////////////////
// Macros for declaration of getter method
#define GETTER_METHOD_DECL(FIELD)									\
	template <typename T>											\
	int getter_##FIELD(lua_State* L) {								\
		CUData* ud = check_userdata<T>(L, 1);						\
		if( !ud->getUser() )										\
			luaL_error( L, "Object destroyed" );					\
		return pushToLua(L, static_cast<T*>(ud->getUser())->FIELD);	\
	}


// Macros for getting the pointer to getter method
#define GETTER_METHOD(ID, FIELD)                          \
	&getter_##FIELD<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define GETTER_METHOD_ENTRY(ID, FIELD)    \
	{ #FIELD, GETTER_METHOD(ID, FIELD) }
//////////////////////////////////////////////////////////////////////////



// Macros for declaration of getter method with specified NAME
#define GETTER_NAMED_METHOD_DECL(NAME, FIELD)                         \
	template <typename T>                                             \
	int getter_##NAME( lua_State* L ){                                \
		CUData* ud = check_userdata<T>( L, 1 );                       \
		if( !ud->getUser() )                                          \
			luaL_error( L, "Object destroyed" );                      \
		return pushToLua( L, static_cast<T*>(ud->getUser())->FIELD ); \
	}


// Macros for declaration of getter method with specified NAME
// If the CHECK fail, then getter will throw lua error.
#define GETTER_NAMED_CHECKED_METHOD_DECL(NAME, CHECK, FIELD)     \
	template <typename T>                                        \
	int getter_##NAME( lua_State* L ){                           \
		CUData* ud = check_userdata<T>(L, 1);                    \
		T* obj = static_cast<T*>(ud->getUser());                 \
		if (!obj)                                                \
			luaL_error(L, "Object destroyed");                   \
		if (!(CHECK))                                            \
			luaL_error(L, "Check not passed");                   \
		return pushToLua(L, obj->FIELD);                         \
	}


// Macros for declaration of getter method with specified NAME
// If the CHECK fail, then getter will return nil.
#define GETTER_NAMED_CHECKED_NIL_METHOD_DECL(NAME, CHECK, FIELD) \
	template <typename T>                                        \
	int getter_##NAME( lua_State* L ){                           \
		CUData* ud = check_userdata<T>(L, 1);                    \
		T* obj = static_cast<T*>(ud->getUser());                 \
		if (!obj)                                                \
			luaL_error(L, "Object destroyed");                   \
		if( !(CHECK) ){                                          \
			lua_pushnil(L);                                      \
			return 1;                                            \
		}                                                        \
		return pushToLua(L, obj->FIELD);                         \
	}


// Macros for getting the pointer to getter method with specified NAME
#define GETTER_NAMED_METHOD(ID, NAME)                         \
	&getter_##NAME<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define GETTER_NAMED_METHOD_ENTRY(ID, NAME)    \
	{ #NAME, GETTER_NAMED_METHOD(ID, NAME) }
//////////////////////////////////////////////////////////////////////////



// Macros for declaration of getset method
#define GETSET_METHOD_DECL(FIELD)                                                               \
	template <typename T>                                                                       \
	int getset_##FIELD( lua_State* L ){                                                         \
		CUData* ud = check_userdata<T>(L, 1);                                                   \
		if (!ud->getUser())                                                                     \
			luaL_error(L, "Object destroyed");                                                  \
		T* obj = static_cast<T*>(ud->getUser());                                                \
		bool typecheck = CHECK_INPUT_PARAM(L, 2, obj->FIELD);                                   \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck failed");                 \
		if (typecheck)                                                                          \
			getFromLua(L, 2, obj->FIELD);                                                       \
		return pushToLua(L, obj->FIELD);                                                        \
	}


// Macros for getting the pointer to getset method
#define GETSET_METHOD(ID, FIELD)                          \
	&getset_##FIELD<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define GETSET_METHOD_ENTRY(ID, FIELD)    \
	{ #FIELD, GETSET_METHOD(ID, FIELD) }
//////////////////////////////////////////////////////////////////////////



// Macros for declaration of getset method with specified NAME
#define GETSET_NAMED_METHOD_DECL(NAME, FIELD)                                                    \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		bool typecheck = CHECK_INPUT_PARAM(L, 2, obj->FIELD);                                    \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck failed");   \
		if (typecheck)                                                                           \
			getFromLua(L, 2, obj->FIELD);                                                        \
		return pushToLua(L, obj->FIELD);                                                         \
	}


// Macros for declaration of getset method with specified NAME
// If the CHECK fail, then getter will throw lua error.
// TYPECHECK is function like lua_isnumber.
#define GETSET_NAMED_CHECKED_METHOD_DECL(NAME, CHECK, FIELD)                                     \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		if (!(CHECK))                                                                            \
			luaL_error(L, "Check not passed");                                                   \
		bool typecheck = CHECK_INPUT_PARAM(L, 2, obj->FIELD);                                    \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck failed");                  \
		if (typecheck)                                                                           \
			getFromLua(L, 2, obj->FIELD);                                                        \
		return pushToLua(L, obj->FIELD);                                                         \
	}


// Macros for declaration of getset method with specified NAME
// If the CHECK fail, then getter will return nil.
#define GETSET_NAMED_CHECKED_NIL_METHOD_DECL(NAME, CHECK, FIELD)                                 \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		if (!(CHECK))                                                                            \
		{                                                                                        \
			lua_pushnil(L);                                                                      \
			return 1;                                                                            \
		}                                                                                        \
		bool typecheck = CHECK_INPUT_PARAM(L, 2, obj->FIELD);                                                   \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck failed");   \
		if (typecheck)                                                                           \
			getFromLua(L, 2, obj->FIELD);                                                        \
		return pushToLua(L, obj->FIELD);                                                         \
	}


// Macros for declaration of getset method fro flag with specified NAME
// FLAG is flagname: in IsDead, the FLAG will be Dead
#define GETSET_FLAG_NAMED_METHOD_DECL(NAME, FLAG)                                                \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		bool typecheck = lua_isboolean(L, 2) != 0;                                               \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck lua_isboolean failed");    \
		if (typecheck)                                                                           \
			getFromLua<bool>(L, 2) ? obj->Set##FLAG() : obj->Clear##FLAG();                      \
		return pushToLua(L, obj->Is##FLAG());                                                    \
	}


// Macros for declaration of getset method with specified NAME
// If the CHECK fail, then getter will throw lua error.
// In obj->sprite->IsVisible(), the OBJ will be sprite
// FLAG is flagname: in IsVisible, the FLAG will be Visible
#define GETSET_FLAG_NAMED_CHECKED_METHOD_DECL(NAME, CHECK, OBJ, FLAG)                          \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		if (!(CHECK))                                                                            \
			luaL_error(L, "Check not passed");                                                   \
		bool typecheck = lua_isboolean(L, 2) != 0;                                               \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck lua_isboolean failed");    \
		if (typecheck)                                                                           \
			getFromLua<bool>(L, 2) ? obj->OBJ->Set##FLAG() : obj->OBJ->Clear##FLAG();            \
		return pushToLua(L, obj->OBJ->Is##FLAG());                                               \
	}


// Macros for declaration of getset method with specified NAME
// If the CHECK fail, then getter will return nil.
// In obj->sprite->IsVisible(), the OBJ will be sprite
// FLAG is flagname: in IsVisible, the FLAG will be Visible
#define GETSET_FLAG_NAMED_CHECKED_NIL_METHOD_DECL(NAME, CHECK, OBJ, FLAG)                        \
	template <typename T>                                                                        \
	int getset_##NAME(lua_State* L)                                                              \
	{                                                                                            \
		CUData* ud = check_userdata<T>(L, 1);                                                    \
		T* obj = static_cast<T*>(ud->getUser());                                                 \
		if (!obj)                                                                                \
			luaL_error(L, "Object destroyed");                                                   \
		if (!(CHECK))                                                                            \
		{                                                                                        \
			lua_pushnil(L);                                                                      \
			return 1;                                                                            \
		}                                                                                        \
		bool typecheck = lua_isboolean(L, 2) != 0;                                               \
		luaL_argcheck(L, typecheck || lua_isnone(L, 2), 2, "typecheck lua_isboolean failed");    \
		if (typecheck)                                                                           \
			getFromLua<bool>(L, 2) ? obj->OBJ->Set##FLAG() : obj->OBJ->Clear##FLAG();            \
		return pushToLua(L, obj->OBJ->Is##FLAG());                                               \
	}


// Macros for getting the pointer to getter method with specified NAME
#define GETSET_NAMED_METHOD(ID, NAME)                         \
	&getset_##NAME<TL::TypeAt<ClassesList, ID>::Result>


// Macros, that puts new method entry in metatable methods array
#define GETSET_NAMED_METHOD_ENTRY(ID, NAME)    \
	{ #NAME, GETSET_NAMED_METHOD(ID, NAME) }
//////////////////////////////////////////////////////////////////////////



// Standard methods for metatble
#define STD_METHODS(i)                                                   \
	{ "__gc",       &on_gc<TL::TypeAt<ClassesList, i>::Result> },        \
	{ "__tostring", &on_tostring<TL::TypeAt<ClassesList, i>::Result> },   \
	{ "object_present", &object_present<TL::TypeAt<ClassesList, i>::Result> }

// Sentinel
#define END \
	{ NULL, NULL }
