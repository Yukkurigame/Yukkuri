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
#include "utils/path.h"
#include "utils/list.h"

#include <cstdlib>
#include <stack>
#include <algorithm>

int ProtoManager::Count = 0;

//FIXME: This is vector but proto id is element number, so it cannot be removed after pushing.
//TODO: make vector a c-array of prototypes
list< Proto* > Prototypes;


void clean_prototypes( )
{
	list< Frame* > deleted;
	ITER_LIST( Proto*, Prototypes ){
		FOREACHIT2( it->data->Actions ){
			Frame* f = it2->second.frames;
			if( f != NULL && !deleted.find( f ) )
				deleted.push_back( f );
		}
		delete it->data;
	}
	ITER_LIST( Frame*, deleted )
		free(it->data);
}


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
	char* path = Path::join( conf.path.proto.c_str(), (name + ".proto").c_str() );

	Debug::debug( Debug::PROTO, "Loading proto: %s.\n", name.c_str() );

	// Файл протоипа будет выполнен в защищенном окружении, чтобы не запороть что-нить глобальное.
	// Окружение создается как здесь: http://community.livejournal.com/ru_lua/402.html
	lua_newtable( Lst );				// Стек: env
	lua_newtable( Lst );				// Стек: env meta
	lua_getglobal( Lst, "_G" );			// Стек: env meta _G
	lua_setfield( Lst, -2, "__index" );	// Стек: env meta
	lua_setmetatable( Lst, -2 );		// Стек: env


	if( luaL_loadfile( Lst, path ) ){
		// Стек: env err
		Debug::debug( Debug::PROTO, "While trying to load proto '%s' (%s): %s.\n",
				name.c_str(), path, lua_tostring( Lst, -1 ) );
		lua_pop( Lst, 2 );	// Стек:
		return 0;
	}
	// Стек: env loadfile
	lua_pushvalue( Lst, -2 );			// Стек: env loadfile env
	lua_setfenv( Lst, -2 );				// Стек: env loadfile

	if( lua_pcall( Lst, 0, 0, 0 ) ){
		// Какая-то ошибка выполнения файла
		Debug::debug( Debug::PROTO, "While trying to execute proto file: %s.\n", lua_tostring( Lst, -1 ) ); // Стек: env err
		Debug::debug( Debug::PROTO, "Unable to load proto '%s'.\n", name.c_str() );
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

	free( path );

	return ++Count;
}



Proto* ProtoManager::GetProtoById( int id )
{
	ITER_LIST( Proto*, Prototypes ){
		if( id == it->data->id  )
			return it->data;
	}
	return NULL;
}


Proto* ProtoManager::GetProtoByName( std::string name )
{
	ITER_LIST( Proto*, Prototypes ){
		if( it->data->name == name )
			return it->data;
	}
	int loaded = LoadPrototype( name );
	if( loaded )
		return Prototypes.tail->data;
	return NULL;
}


void ProtoManager::LoadTable(lua_State* L, Proto* proto)
{
	// Load physics
	// Стек: env
	lua_getfield( L, -1, "physics" );	// Стек: env actions
	if( lua_istable(L, -1) ){
		getValueByName( L, "shape", proto->physicsType );
		getValueByName( L, "static", proto->statical );
	}else{
		proto->physicsType = 0;
		proto->statical = true;
	}
	lua_pop(L, 1);						// Стек: env

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

				const int action_id = Action::getId( aname );

				Action* a = &proto->Actions[action_id];
				a->id = action_id;

				lua_getfield(L, -1, "frames");	// Стек: env actions key actions[key] frames
				if( lua_istable(L, -1) ){
					a->framesCount = lua_objlen(L, -1);
					a->frames = NULL;

					if( a->framesCount > 0 ){
						a->frames = (Frame*)malloc( (unsigned)sizeof(Frame) * a->framesCount );

						size_t j = 0;
						std::stack< Frame* > conditions;

						// Стек: env actions key actions[key] frames nil
						for( lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1), j++ ){
							// Стек: env actions key actions[key] frames key frame
							assert( j < a->framesCount );
							if( lua_istable( L, -1 ) ){
								UINT command = acNone;
								Frame& frame = a->frames[j];

								frame.condition_end = a->framesCount;

								for(int fr = 0; fr < FRAME_PARAMS_COUNT; ++fr ){
									frame.params[fr].stringData = NULL;
									frame.param_types[fr] = stNone;
								}

								getValueByName( L, "num", frame.num );
								getValueByName( L, "dur", frame.duration );
								getValueByName( L, "skip", frame.skip_on_delay );

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
										// FIXME: Dirty hack.
										// Check if parameter is first parameter to setAction or super
										// command and interpret it as action id
										if( !params_added &&
											( command == acSuper || command == acSetAction ) &&
											lua_isstring( L, -1 ) ){
											std::string param_aname = lua_tostring( L, -1 );
											frame.param_types[params_added] = stInt;
											frame.params[params_added].intData = Action::getId( param_aname );
										}else if( lua_isnumber( L, -1 ) ){
											frame.param_types[params_added] = stInt;
											frame.params[params_added].intData = (int)lua_tonumber( L, -1 );
										}else if( lua_isstring( L, -1 ) ){
											frame.param_types[params_added] = stString;
											frame.params[params_added].stringData = lua_tostring( L, -1 );
										}else if( lua_istable( L, -1 ) ){
											frame.param_types[params_added] = stTable;
											// This will remove table from stack.
											RegObject( L, &(frame.params[params_added].intData), -1 );
											// We need to add one value for lua_next
											lua_pushnil( L );
										}else if( lua_isfunction( L, -1 ) ){
											frame.param_types[params_added] = stFunction;
											// This will remove function from stack.
											RegProc( L, &(frame.params[params_added].intData), -1 );
											// We need to add one value for lua_next
											lua_pushnil( L );
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
										Debug::debug( Debug::PROTO,
												"Unexpected acEnd command at frame %d in action '%s' of proto '%s'. Skipping.\n",
												j, Action::getName(a->id).c_str(), proto->name.c_str() );
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
								}

							}else{
								Debug::debug( Debug::PROTO,
									"In proto '%s', action '%s' frame %d  - %s: Frame loading canceled (not a table).\n",
									proto->name.c_str(), Action::getName(a->id).c_str(), j, lua_typename(L, lua_type(L, -1)) );
								a->framesCount = 0;
							}
							//lua_pop(L, 1);	// Стек: env actions key actions[key] frames key
						}

						while( !conditions.empty() ){
							Frame* f = conditions.top();
							Debug::debug( Debug::PROTO,
								"End of %d condition expected. In proto '%s', action '%s'.\n",
								f->command, proto->name.c_str(), Action::getName(a->id).c_str() );
							f->condition_end = a->framesCount;
							conditions.pop();
						}
					}else{
						Debug::debug( Debug::PROTO,
							"There are no frames in action '%s' of proto '%s'. Skipping.\n",
							Action::getName(a->id).c_str(), proto->name.c_str() );
						a->framesCount = 0;
					}

					// Стек: env actions key actions[key] frames

				}else{ // if (lua_istable(L, -1))	frames
					Debug::debug( Debug::PROTO,
						"Action '%s' in proto '%s' have no frames table. Skipping.\n",
						Action::getName(a->id).c_str(), proto->name.c_str() );
					a->framesCount = 0;
				}

				// Стек: env animations key animations[key] frames
				lua_pop(L, 1);	// Стек: env actions key actions[key]
			}else{
				Debug::debug( Debug::PROTO, "In proto '%s' actions[%d] is %s.\n",
						proto->name.c_str(), i, lua_typename(L, lua_type(L, -1)) );
			}

		} // for


		// Стек: env actions

	} // if (lua_istable(L, -1))   actions

	lua_pop(L, 1);						// Стек: env

}


