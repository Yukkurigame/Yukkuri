/*
 * proto.cpp
 *
 *  Created on: 25.04.2012
 *
 */

#include "proto.h"
#include "debug.h"
#include "config.h"
#include "assert.h"
#include "hacks.h"

#include <cstdlib>

int ProtoManager::Count = 0;

std::vector<Proto> Prototypes;

ProtoManager::ProtoManager()
{

}

ProtoManager::~ProtoManager()
{
}


int ProtoManager::LoadPrototype( std::string name )
{
	Proto p;
	extern MainConfig conf;
	LuaStackChecker sc( Lst, __FILE__, __LINE__);

	p.name = name;
	p.id = -1;

	Debug::debug( Debug::CONFIG, "Loading proto: " + name + ".\n");

	// Файл протоипа будет выполнен в защищенном окружении, чтобы не запороть что-нить глобальное.
	// Окружение создается как здесь: http://community.livejournal.com/ru_lua/402.html
	lua_newtable( Lst );				// Стек: env
	lua_newtable( Lst );				// Стек: env meta
	lua_getglobal( Lst, "_G" );			// Стек: env meta _G
	lua_setfield( Lst, -2, "__index" );	// Стек: env meta
	lua_setmetatable( Lst, -2 );		// Стек: env


	if( luaL_loadfile( Lst, (conf.protoPath + name).c_str()) ){
		// Стек: env err
		Debug::debug( Debug::CONFIG, "While trying to load proto '" + name + "': " + lua_tostring( Lst, -1 ) + ".\n");
		lua_pop( Lst, 2 );	// Стек:
		return 0;
	}
	// Стек: env loadfile
	lua_pushvalue( Lst, -2 );			// Стек: env loadfile env
	lua_setfenv( Lst, -2 );				// Стек: env loadfile

	if( lua_pcall( Lst, 0, 0, 0 ) ){
		// Какая-то ошибка выполнения файла
		Debug::debug( Debug::CONFIG, lua_tostring( Lst, -1 ) ); // Стек: env err
		Debug::debug( Debug::CONFIG, "Unable to load proto '" + name + "'.\n" );
		lua_pop( Lst, 2 );	// Стек:

		return 0;
	}else{
		LoadTable( Lst, &p );
	} // if lua_pcall

	lua_pop( Lst, 1 );	// Стек:

	p.id = Count;

	Prototypes.push_back(p);

	return ++Count;
}



Proto ProtoManager::GetProtoById( int id )
{
	if( id >= 0 && id < (int)Prototypes.size() )
		return Prototypes[id];
	Proto p; //FUU
	return p;
}


Proto ProtoManager::GetProtoByName( std::string name )
{
	FOREACHIT( Prototypes ){
		if( it->name == name )
			return Prototypes[it->id];
	}
	int loaded = LoadPrototype( name );
	if( loaded )
		return Prototypes[loaded - 1];
	Proto p; //FUU
	return p;
}


void ProtoManager::LoadTable(lua_State* L, Proto* proto)
{
	LoadActions( L, proto );
}

void ProtoManager::LoadActions(lua_State* L, Proto* proto)
{
	LuaStackChecker sc( Lst, __FILE__, __LINE__);

	std::string parentname;

	getValueByName( L, "parent", parentname );

	if( parentname != "" ){
		Proto parent = GetProtoByName( parentname );
		if( parent.id >= 0 )
			proto->Actions.insert( parent.Actions.begin(), parent.Actions.end() );
	}

	// Стек: env
	lua_getfield( L, -1, "actions" );	// Стек: env actions
	if( lua_istable(L, -1) ){
		const size_t count = lua_objlen( L, -1 );
		if( count == 0 ){
			lua_pop( L, 1 );			// Стек: env
			return;
		}

		Action a;

		// Цикл по массиву действий
		size_t i = 0;

		// Стек: env actions nil
		for( lua_pushnil(L); lua_next( L, -2 ); lua_pop( L, 1 ), ++i ){
			// Стек: env actions key actions[key]
			if( lua_istable( L, -1 ) ){
				// Получили таблицу прототипа анимации, читаем ее

				getValueByName( L, "name", a.name );

				lua_getfield(L, -1, "frames");	// Стек: env actions key actions[key] frames
				if( lua_istable(L, -1) ){
					a.framesCount = lua_objlen(L, -1);
					if( a.framesCount > 0 ){
						a.frames = (Frame*)malloc( sizeof(Frame) * a.framesCount );

						size_t j = 0;

						// Стек: env actions key actions[key] frames nil
						for( lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1), j++ ){
							// Стек: env actions key actions[key] frames key frame
							assert( j < a.framesCount );
							if( lua_istable( L, -1 ) ){
								UINT command;

								getValueByName( L, "num", a.frames[j].num );
								getValueByName( L, "dur", a.frames[j].duration );
								getValueByName( L, "com", command );
								a.frames[j].command = (enum ActionCommand)command;

								lua_getfield( L, -1, "param" ); // st: ... frames key frame param
								if( lua_isfunction( L, -1 ) ){
									RegProc( L, static_cast<LuaRegRef*>( &(a.frames[j].param) ), -1 );
									a.frames[j].is_param_function = true;

									// st: ... frames key frame
								}else{
									lua_pop( L, 1 ); // st: ... frames key frame
									getValueByName( L, "param", a.frames[j].param );
								}

								getValueByName( L, "text", a.frames[j].txt_param );
							}else{
								char* dbg;
								sprintf( dbg, "frame %d - %s: Frame loading canceled (not a table).\n", (int)j, lua_typename(L, lua_type(L, -1)) );
								Debug::debug( Debug::CONFIG,
										"In proto '" + proto->name + "', action '" + a.name + "' " + dbg );
								a.framesCount = 0;
								free(dbg);
							}
							//lua_pop(L, 1);	// Стек: env actions key actions[key] frames key
						}
					}else{
						Debug::debug( Debug::CONFIG, "There are no frames in action '" + a.name + "' of proto '" + proto->name + "'. Skipping.\n");
						a.framesCount = 0;
					}

					// Стек: env actions key actions[key] frames

				}else{ // if (lua_istable(L, -1))	frames
					Debug::debug( Debug::CONFIG, "Action '" + a.name + "' in proto '" + proto->name + "' have no frames table. Skipping.\n");
					a.framesCount = 0;
				}

				if( a.framesCount > 0){
					proto->Actions[a.name] = a;
				}

				// Стек: env animations key animations[key] frames
				lua_pop(L, 1);	// Стек: env actions key actions[key]
			}else{
				char* dbg;
				sprintf( dbg, "actions[%d]", (int)i );
				Debug::debug( Debug::CONFIG, "In proto '" + proto->name + "' " + dbg + " is " + lua_typename(L, lua_type(L, -1)) );
				free(dbg);
			}

		} // for


		// Стек: env actions

	} // if (lua_istable(L, -1))   actions

	lua_pop(L, 1);						// Стек: env

}


