/*
 * proto.h
 *
 *  Created on: 25.04.2012
 *
 */

#ifndef PROTO_H_
#define PROTO_H_

#include "scripts/LuaScript.h"
#include "units/Prototypes.h"
#include "types.h"


void clean_prototypes( );


class ProtoManager : public LuaScript
{
public:

	ProtoManager();
	~ProtoManager();

	int LoadPrototype( std::string name );

	Proto* GetProtoById( int id );
	Proto* GetProtoByName( std::string name );

	void LoadTable( lua_State* L, Proto* proto );

	void LoadActions(lua_State* L, Proto* proto);


private:
	static int Count;
};


#endif /* PROTO_H_ */
