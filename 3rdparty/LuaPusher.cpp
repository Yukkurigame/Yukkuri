/*
 * LuaPusher.cpp
 *
 *  Created on: 20.05.2012
 *
 * Origin: iiCHantra
 *
 * I'm too lazy to rewrite it myself
 *
 */


#include "3rdparty/LuaPusher.h"
#include "scripts/Lua.h"
#include "assert.h"
#include <cstring>
#include "types.h"
#include "utf.h"

// int
template <>
int pushToLua(lua_State* L, int const & val)
{
	lua_pushinteger(L, val);
	return 1;
}

template<>
int getFromLua(lua_State* L, int idx)
{
	return static_cast<int>(lua_tointeger(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, int& val)
{
	val = static_cast<int>(lua_tointeger(L, idx));
}

template<>
bool CHINP_TESTER<int>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// unsigned int
template <>
int pushToLua(lua_State* L, unsigned int const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
unsigned int getFromLua(lua_State* L, int idx)
{
	return static_cast<unsigned int>(lua_tonumber(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, unsigned int& val)
{
	val = static_cast<unsigned int>(lua_tonumber(L, idx));
}

template<>
bool CHINP_TESTER<unsigned int>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// long
template <>
int pushToLua(lua_State* L, long const & val)
{
	lua_pushinteger(L, val);
	return 1;
}

template<>
long getFromLua(lua_State* L, int idx)
{
	return static_cast<long>(lua_tointeger(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, long& val)
{
	val = static_cast<long>(lua_tointeger(L, idx));
}

template<>
bool CHINP_TESTER<long>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// unsigned long
template <>
int pushToLua(lua_State* L, unsigned long const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
unsigned long getFromLua(lua_State* L, int idx)
{
	return static_cast<unsigned long>(lua_tonumber(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, unsigned long& val)
{
	val = static_cast<unsigned long>(lua_tonumber(L, idx));
}

template<>
bool CHINP_TESTER<unsigned long>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// short
template <>
int pushToLua(lua_State* L, short const & val)
{
	lua_pushinteger(L, val);
	return 1;
}

template<>
short getFromLua(lua_State* L, int idx)
{
	return static_cast<short>(lua_tointeger(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, short& val)
{
	val = static_cast<short>(lua_tointeger(L, idx));
}

template<>
bool CHINP_TESTER<short>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// unsigned short
template <>
int pushToLua(lua_State* L, unsigned short const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
unsigned short getFromLua(lua_State* L, int idx)
{
	return static_cast<unsigned short>(lua_tonumber(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, unsigned short& val)
{
	val = static_cast<unsigned short>(lua_tonumber(L, idx));
}

template<>
bool CHINP_TESTER<unsigned short>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// float
template <>
int pushToLua(lua_State* L, float const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
float getFromLua(lua_State* L, int idx)
{
	return static_cast<float>(lua_tonumber(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, float& val)
{
	val = static_cast<float>(lua_tonumber(L, idx));
}

template<>
bool CHINP_TESTER<float>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// double
template <>
int pushToLua(lua_State* L, double const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
double getFromLua(lua_State* L, int idx)
{
	return static_cast<double>(lua_tonumber(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, double& val)
{
	val = static_cast<double>(lua_tonumber(L, idx));
}

// char
template <>
int pushToLua(lua_State* L, char const & val)
{
	lua_pushinteger(L, val);
	return 1;
}

// unsigned char
template <>
int pushToLua(lua_State* L, unsigned char const & val)
{
	lua_pushnumber(L, val);
	return 1;
}

template<>
void getFromLua(lua_State* L, int idx, unsigned char& val)
{
	val = static_cast<unsigned char>(lua_tointeger(L, idx));
}

template<>
bool CHINP_TESTER<unsigned char>(lua_State* L, int idx)
{
	return lua_isnumber(L, idx) != 0;
}

// bool
template <>
int pushToLua(lua_State* L, bool const & val)
{
	lua_pushboolean(L, val);
	return 1;
}

template<>
bool getFromLua(lua_State* L, int idx)
{
	return lua_toboolean(L, idx) != 0;
}

template<>
void getFromLua(lua_State* L, int idx, bool& val)
{
	val = (lua_toboolean(L, idx) != 0);
}

template<>
bool CHINP_TESTER<bool>(lua_State* L, int idx)
{
	return lua_isboolean(L, idx) != 0;
}

// const char*
template <>
int pushToLua(lua_State* L, const char* const & val)
{
	lua_pushstring(L, val);
	return 1;
}

template<>
const char* getFromLua(lua_State* L, int idx)
{
	return lua_tostring(L, idx);
}

template<>
void getFromLua(lua_State* L, int idx, const char * & val)
{
	val = lua_tostring(L, idx);
}

template<>
bool CHINP_TESTER<const char *>(lua_State* L, int idx)
{
	return lua_isstring(L, idx) != 0;
}

// char*
template <>
int pushToLua(lua_State* L, char* const & val)
{
	lua_pushstring(L, val);
	return 1;
}

template<>
char* getFromLua(lua_State* L, int idx)
{
	return strdup(lua_tostring(L, idx));
}

template<>
void getFromLua(lua_State* L, int idx, char * & val)
{
	// If it failed here, there is, probably, memory leak.
	// Please, be very careful to pass non-NULL pointers here.
	assert(val == NULL);
	val = strdup(lua_tostring(L, idx));
}

template<>
bool CHINP_TESTER<char *>(lua_State* L, int idx)
{
	return lua_isstring(L, idx) != 0;
}

// std::string
template <>
int pushToLua(lua_State* L, std::string const & val)
{
	lua_pushstring(L, val.c_str());
	return 1;
}

template<>
std::string getFromLua(lua_State* L, int idx)
{
	size_t size = 0;
	const char* s = lua_tolstring(L, idx, &size);
	return std::string(s, size);
}

template<>
void getFromLua(lua_State* L, int idx, std::string& val)
{
	size_t size = 0;
	const char* s = lua_tolstring(L, idx, &size);
	val.assign(s, size);
}

template<>
bool CHINP_TESTER<std::string>(lua_State* L, int idx)
{
	return lua_isstring(L, idx) != 0;
}


// std::wstring
template <>
int pushToLua( lua_State* L, std::wstring const & val )
{
	lua_pushstring(L, UTF8_to_str( val.c_str() ) );
	return 1;
}

template<>
std::wstring getFromLua( lua_State* L, int idx )
{
	size_t size = 0;
	const wchar_t* s = str_to_UTF8( lua_tolstring(L, idx, &size) );
	return std::wstring(s, size);
}

template<>
void getFromLua( lua_State* L, int idx, std::wstring& val )
{
	size_t size = 0;
	const wchar_t* s = str_to_UTF8( lua_tolstring(L, idx, &size) );
	val.assign( s, size );
}

template<>
bool CHINP_TESTER<std::wstring>( lua_State* L, int idx )
{
	return lua_isstring(L, idx) != 0;
}


//void
template <>
int pushToLua(lua_State* L, const LuaRet& r )
{
	return r.c;
}
