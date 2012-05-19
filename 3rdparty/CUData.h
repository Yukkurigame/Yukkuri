/*
 * CUData.h
 *
 *  Created on: 19.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef CUDATA_H_3rd
#define CUDATA_H_3rd

struct lua_State;
class CUData;

void RegisterAllTypeMetatables(lua_State* L);


template <typename T>
CUData* CreateUData(lua_State* L, T& user);

template <typename T>
CUData* check_userdata(lua_State* L, int ud, bool throw_error = true );


#endif /* CUDATA_H_3rd */
