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
#include "safestring.h"

#include <cstdlib>
#include <stack>

int ProtoManager::Count = 0;

//FIXME: This is vector but proto id is element number, so it cannot be removed after pushing.
//TODO: make vector a c-array of prototypes
std::vector<Proto*> Prototypes;


ProtoManager::ProtoManager()
{

}

ProtoManager::~ProtoManager()
{
}


int ProtoManager::LoadPrototype( std::string name )
{
	Proto* p = new Proto;
	extern MainConfig conf;
	LuaStackChecker sc( Lst, __FILE__, __LINE__);

	p->name = name;
	p->id = -1;
	std::string path = conf.protoPath + name + ".proto";

	Debug::debug( Debug::CONFIG, "Loading proto: " + name + ".\n");

	// Файл протоипа будет выполнен в защищенном окружении, чтобы не запороть что-нить глобальное.
	// Окружение создается как здесь: http://community.livejournal.com/ru_lua/402.html
	lua_newtable( Lst );				// Стек: env
	lua_newtable( Lst );				// Стек: env meta
	lua_getglobal( Lst, "_G" );			// Стек: env meta _G
	lua_setfield( Lst, -2, "__index" );	// Стек: env meta
	lua_setmetatable( Lst, -2 );		// Стек: env


	if( luaL_loadfile( Lst, path.c_str()) ){
		// Стек: env err
		Debug::debug( Debug::CONFIG, "While trying to load proto '" + name + "' (" +
				path + "): " + lua_tostring( Lst, -1 ) + ".\n");
		lua_pop( Lst, 2 );	// Стек:
		return 0;
	}
	// Стек: env loadfile
	lua_pushvalue( Lst, -2 );			// Стек: env loadfile env
	lua_setfenv( Lst, -2 );				// Стек: env loadfile

	if( lua_pcall( Lst, 0, 0, 0 ) ){
		// Какая-то ошибка выполнения файла
		Debug::debug( Debug::CONFIG, std::string(lua_tostring( Lst, -1 )) + ".\n" ); // Стек: env err
		Debug::debug( Debug::CONFIG, "Unable to load proto '" + name + "'.\n" );
		lua_pop( Lst, 2 );	// Стек:

		return 0;
	}else{
		std::string parentname;
		getValueByName( Lst, "parent", parentname );
		if( parentname != "" )
			p->parent = GetProtoByName( parentname );
		LoadTable( Lst, p );
	} // if lua_pcall

	lua_pop( Lst, 1 );	// Стек:

	p->id = Count;

	Prototypes.push_back(p);

	return ++Count;
}



Proto* ProtoManager::GetProtoById( int id )
{
	if( id >= 0 && id < (int)Prototypes.size() )
		return Prototypes[id];
	return NULL;
}


Proto* ProtoManager::GetProtoByName( std::string name )
{
	FOREACHIT( Prototypes ){
		if( (*it)->name == name )
			return Prototypes[(*it)->id];
	}
	int loaded = LoadPrototype( name );
	if( loaded )
		return Prototypes[loaded - 1];
	return NULL;
}


void ProtoManager::LoadTable(lua_State* L, Proto* proto)
{
	LoadActions( L, proto );
}

void ProtoManager::LoadActions(lua_State* L, Proto* proto)
{
	LuaStackChecker sc( Lst, __FILE__, __LINE__);

	if( !proto )
		return;

	if( proto->parent != NULL )
			proto->Actions.insert( proto->parent->Actions.begin(), proto->parent->Actions.end() );

	// Стек: env
	lua_getfield( L, -1, "actions" );	// Стек: env actions
	if( lua_istable(L, -1) ){
		const size_t count = lua_objlen( L, -1 );
		if( count == 0 ){
			lua_pop( L, 1 );			// Стек: env
			return;
		}

		// Цикл по массиву действий
		size_t i = 0;

		// Стек: env actions nil
		for( lua_pushnil(L); lua_next( L, -2 ); lua_pop( L, 1 ), ++i ){
			// Стек: env actions key actions[key]
			if( lua_istable( L, -1 ) ){
				// Получили таблицу прототипа анимации, читаем ее

				std::string aname;

				getValueByName( L, "name", aname );



				Action* a = &proto->Actions[aname];
				a->name = aname;

				lua_getfield(L, -1, "frames");	// Стек: env actions key actions[key] frames
				if( lua_istable(L, -1) ){
					a->framesCount = lua_objlen(L, -1);
					if( a->framesCount > 0 ){
						a->frames = (Frame*)malloc( sizeof(Frame) * a->framesCount );

						size_t j = 0;
						std::stack< Frame* > conditions;

						// Стек: env actions key actions[key] frames nil
						for( lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1), j++ ){
							// Стек: env actions key actions[key] frames key frame
							assert( j < a->framesCount );
							if( lua_istable( L, -1 ) ){
								UINT command = acNone;
								Frame& frame = a->frames[j];

								frame.repeat = 0;

								for(int fr = 0; fr < FRAME_PARAMS_COUNT; ++fr ){
									frame.params[fr].stringData = NULL;
									frame.param_types[fr] = stNone;
								}

								getValueByName( L, "num", frame.num );
								getValueByName( L, "dur", frame.duration );

								// Add non-keyworded parameters
								int params_added = 0;
								for( lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1) ){
									// Стек: env ... frames key frame nil
									if( params_added >= FRAME_PARAMS_COUNT )
										break;
									if( !lua_isnumber( L, -2 ) )
										continue;
									if( lua_tonumber( L, -2 ) == 1 ){
										command = lua_tointeger( L, -1 );
									}else{
										if( lua_isnumber( L, -1 ) ){
											frame.param_types[params_added] = stInt;
											frame.params[params_added].intData = lua_tonumber( L, -1 );
										}else if( lua_isstring( L, -1 )){
											frame.param_types[params_added] = stString;
											frame.params[params_added].stringData = lua_tostring( L, -1 );
										}else
											continue;
										params_added++;
									}
								}

								frame.command = (enum ActionCommand)command;

								// Add condition to condition stack.
								if( command >= acCondition && command < acEnd )
									conditions.push( &a->frames[j] );
								else if( command == acEnd ){
									if( conditions.empty() )
										Debug::debug( Debug::CONFIG, "Unexpected acEnd command at frame " + citoa(j) +
											" in action '" + a->name + "' of proto '" + proto->name + "'. Skipping.\n");
									else{
										conditions.top()->condition_end = j;
										conditions.pop();
									}
								}

								lua_getfield( L, -1, "func" ); // st: ... frames key frame param
								if( lua_isfunction( L, -1 ) ){
									RegProc( L, &frame.func, -1 );
									// st: ... frames key frame
								}else{
									lua_pop( L, 1 ); // st: ... frames key frame
									frame.func = LUA_NOREF;
									//getValueByName( L, "param", frame.param );
								}

								//getValueByName( L, "text", frame.txt_param );
							}else{
								Debug::debug( Debug::CONFIG,
									"In proto '" + proto->name + "', action '" + a->name + "' " +
									"frame " + citoa(j) + " - " + lua_typename(L, lua_type(L, -1)) +
									": Frame loading canceled (not a table).\n");
								a->framesCount = 0;
							}
							//lua_pop(L, 1);	// Стек: env actions key actions[key] frames key
						}

						while( !conditions.empty() ){
							Frame* f = conditions.top();
							Debug::debug( Debug::CONFIG,
									"End of " + citoa(f->command) + " condition expected. In proto '" + proto->name +
									"', action '" + a->name + "'.\n");
							f->condition_end = a->framesCount;
							conditions.pop();
						}
					}else{
						Debug::debug( Debug::CONFIG, "There are no frames in action '" +
								a->name + "' of proto '" + proto->name + "'. Skipping.\n");
						a->framesCount = 0;
					}

					// Стек: env actions key actions[key] frames

				}else{ // if (lua_istable(L, -1))	frames
					Debug::debug( Debug::CONFIG, "Action '" + a->name + "' in proto '" +
							proto->name + "' have no frames table. Skipping.\n");
					a->framesCount = 0;
				}

				// Стек: env animations key animations[key] frames
				lua_pop(L, 1);	// Стек: env actions key actions[key]
			}else{
				Debug::debug( Debug::CONFIG, "In proto '" + proto->name + "' actions[" +
						citoa(i) + "] is " + lua_typename(L, lua_type(L, -1)) );
			}

		} // for


		// Стек: env actions

	} // if (lua_istable(L, -1))   actions

	lua_pop(L, 1);						// Стек: env

}


