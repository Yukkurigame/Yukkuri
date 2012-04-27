/*
 * proto.h
 *
 *  Created on: 25.04.2012
 *
 */

#ifndef PROTO_H_
#define PROTO_H_

#include "LuaScript.h"
#include "Actions.h"
#include "types.h"

struct Proto
{
	int id;
	std::string name;
	std::string parent;

	std::map< std::string, Action > Actions;

	Proto() : id(-1) {};
};

class ProtoManager : public LuaScript
{
public:

	ProtoManager();
	~ProtoManager();

	int LoadPrototype( std::string name );

	Proto GetProtoById( int id );
	Proto GetProtoByName( std::string name );

	void LoadTable( lua_State* L, Proto* proto );

	void LoadActions(lua_State* L, Proto* proto);


private:
	static int Count;
};


#endif /* PROTO_H_ */
