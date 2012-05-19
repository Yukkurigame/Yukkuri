/*
 * CUData.h
 *
 *  Created on: 19.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef CUDATATEMPLATES_H_
#define CUDATATEMPLATES_H_

struct lua_State;
class CUData;

void RegisterAllTypeMetatables(lua_State* L);


template <typename T>
CUData* CreateUData(lua_State* L, T& user);

template <typename T>
CUData* check_userdata(lua_State* L, int ud, bool throw_error = true );


#endif /* CUDATATEMPLATES_H_ */
