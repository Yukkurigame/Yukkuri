/*
 * LuaPusher.h
 *
 *  Created on: 20.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef LUAPUSHER_H_
#define LUAPUSHER_H_


struct lua_State;

#define UNUSED_ARG(x)	(void)x

/// Pushes value of type T onto lua stack
/// @return the count of pushed on stack values
template <typename T>
int pushToLua(lua_State* L, T const & val);

/// Returns the value of type T located on lua stack
/// @param idx is the position of value on lua stack
template <typename T>
T getFromLua(lua_State* L, int idx);

/// Returns the value of type T located on lua stack
/// @param idx is the position of value on lua stack
/// @param[out] val is the returned value
template <typename T>
void getFromLua(lua_State* L, int idx, T& val);

template <typename T>
T returnFromLua(lua_State* L, int idx, T& val);

// Uncomment this if your compiler knows how to use decltype keyword
//#define USE_DECLTYPE
#if defined(USE_DECLTYPE)

#	define CHINP_TESTER checkInputParam
#	define CHECK_INPUT_PARAM(L, IDX, PARAM) \
		CHINP_TESTER<decltype(PARAM)>(L, IDX)
#else

#	define CHINP_HELPER checkInputParam
#	define CHINP_TESTER checkInputParamTester
#	define CHECK_INPUT_PARAM(L, IDX, PARAM) \
		CHINP_HELPER(L, IDX, PARAM)

	template <typename T>
	bool CHINP_HELPER(lua_State* L, int idx, T const & val);

#endif // USE_DECLTYPE

/// Performs the testing of value on stack.
/// @param idx is the position of value on lua stack
/// @return true if value on stack can be (probably) converted into type T
template <typename T>
bool CHINP_TESTER(lua_State* L, int idx);

#if defined(CHINP_HELPER)
	// Used if compiler too old to use decltype, so we need to bring some template magic.
	template <typename T>
	bool CHINP_HELPER(lua_State* L, int idx, T const & val)
	{
		UNUSED_ARG(val);
		return CHINP_TESTER<T>(L, idx);
	}
#endif // CHINP_HELPER


#endif /* LUAPUSHER_H_ */
