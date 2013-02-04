/*
 * Bindings.cpp
 *
 *  Created on: 07.07.2010
 */

#include "debug.h"

#include "SDL/SDL_keysym.h"

#include "Bindings.h"
#include "scripts/LuaScript.h"
#include "scripts/LuaScriptConfig.h"
#include "utils/list.h"
#include <cstring>
#include <cstdlib>

#include <map>
#include "hacks.h"


extern LuaScript* luaScript;


namespace {

	BindFunction BindedFunctions[ MAXKEYS ];
	const char* KeyNames[ MAXKEYS ];
	const char* Current;
	list< UINT > BindedKeys;
	LuaRegRef Reciever;

	const char* none_string = "\0";
}



void Bindings::init( )
{
	Reciever = LUA_NOREF;

	bindKey( SDLK_BACKSPACE, "backspace" );
	bindKey( SDLK_TAB, "tab" );
	// SDLK_CLEAR = 12,
	bindKey( SDLK_RETURN, "enter" );
	bindKey( SDLK_PAUSE, "pause" );
	bindKey( SDLK_ESCAPE, "esc" );
	bindKey( SDLK_SPACE, "space" );
	bindKey( SDLK_EXCLAIM, "exclaim" );
	bindKey( SDLK_QUOTEDBL, "quotedbl" );
	bindKey( SDLK_HASH, "hash" );
	bindKey( SDLK_DOLLAR, "dollar" );
	bindKey( SDLK_AMPERSAND, "ampersand" );
	bindKey( SDLK_QUOTE, "quote" );
	bindKey( SDLK_LEFTPAREN, "leftparen" );
	bindKey( SDLK_RIGHTPAREN, "rightparen" );
	bindKey( SDLK_ASTERISK, "asterisk" );
	bindKey( SDLK_PLUS, "plus" );
	bindKey( SDLK_COMMA, "comma" );
	bindKey( SDLK_MINUS, "minus" );
	bindKey( SDLK_PERIOD, "period" );
	bindKey( SDLK_SLASH, "slash" );

	bindKey( SDLK_0, "0" );
	bindKey( SDLK_1, "1" );
	bindKey( SDLK_2, "2" );
	bindKey( SDLK_3, "3" );
	bindKey( SDLK_4, "4" );
	bindKey( SDLK_5, "5" );
	bindKey( SDLK_6, "6" );
	bindKey( SDLK_7, "7" );
	bindKey( SDLK_8, "8" );
	bindKey( SDLK_9, "9" );
	bindKey( SDLK_COLON, "colon" );
	bindKey( SDLK_SEMICOLON, "semicolon" );
	bindKey( SDLK_LESS, "less" );
	bindKey( SDLK_EQUALS, "equals" );
	bindKey( SDLK_GREATER, "greater" );
	bindKey( SDLK_QUESTION, "question" );
	bindKey( SDLK_AT, "at" );

	bindKey( SDLK_LEFTBRACKET, "lbracket" );
	bindKey( SDLK_BACKSLASH, "backslash" );
	bindKey( SDLK_RIGHTBRACKET, "rbracket" );
	bindKey( SDLK_CARET, "caret" );
	bindKey( SDLK_UNDERSCORE, "underscore" );
	bindKey( SDLK_BACKQUOTE, "backquote" );
	bindKey( SDLK_a, "a" );
	bindKey( SDLK_b, "b" );
	bindKey( SDLK_c, "c" );
	bindKey( SDLK_d, "d" );
	bindKey( SDLK_e, "e" );
	bindKey( SDLK_f, "f" );
	bindKey( SDLK_g, "g" );
	bindKey( SDLK_h, "h" );
	bindKey( SDLK_i, "i" );
	bindKey( SDLK_j, "j" );
	bindKey( SDLK_k, "k" );
	bindKey( SDLK_l, "l" );
	bindKey( SDLK_m, "m" );
	bindKey( SDLK_n, "n" );
	bindKey( SDLK_o, "o" );
	bindKey( SDLK_p, "p" );
	bindKey( SDLK_q, "q" );
	bindKey( SDLK_r, "r" );
	bindKey( SDLK_s, "s" );
	bindKey( SDLK_t, "t" );
	bindKey( SDLK_u, "u" );
	bindKey( SDLK_v, "v" );
	bindKey( SDLK_w, "w" );
	bindKey( SDLK_x, "x" );
	bindKey( SDLK_y, "y" );
	bindKey( SDLK_z, "z" );
	bindKey( SDLK_DELETE, "delete" );

	/* International keyboard syms later */

	bindKey( SDLK_KP0, "num0" );
	bindKey( SDLK_KP1, "num1" );
	bindKey( SDLK_KP2, "num2" );
	bindKey( SDLK_KP3, "num3" );
	bindKey( SDLK_KP4, "num4" );
	bindKey( SDLK_KP5, "num5" );
	bindKey( SDLK_KP6, "num6" );
	bindKey( SDLK_KP7, "num7" );
	bindKey( SDLK_KP8, "num8" );
	bindKey( SDLK_KP9, "num9" );
	bindKey( SDLK_KP_PERIOD, "decimal" );
	bindKey( SDLK_KP_DIVIDE, "divide" );
	bindKey( SDLK_KP_MULTIPLY, "multiply" );
	bindKey( SDLK_KP_MINUS, "kpminus" );
	bindKey( SDLK_KP_PLUS, "kpplus" );
	bindKey( SDLK_KP_ENTER, "kpenter" );
	bindKey( SDLK_KP_EQUALS, "equals" );

	bindKey( SDLK_UP, "up" );
	bindKey( SDLK_DOWN, "down" );
	bindKey( SDLK_RIGHT, "right" );
	bindKey( SDLK_LEFT, "left" );
	bindKey( SDLK_INSERT, "insert" );
	bindKey( SDLK_HOME, "home" );
	bindKey( SDLK_END, "end" );
	bindKey( SDLK_PAGEUP, "pgup" );
	bindKey( SDLK_PAGEDOWN, "pgdown" );

	bindKey( SDLK_F1, "f1" );
	bindKey( SDLK_F2, "f2" );
	bindKey( SDLK_F3, "f3" );
	bindKey( SDLK_F4, "f4" );
	bindKey( SDLK_F5, "f5" );
	bindKey( SDLK_F6, "f6" );
	bindKey( SDLK_F7, "f7" );
	bindKey( SDLK_F8, "f8" );
	bindKey( SDLK_F9, "f9" );
	bindKey( SDLK_F10, "f10" );
	bindKey( SDLK_F11, "f11" );
	bindKey( SDLK_F12, "f12" );
	bindKey( SDLK_F13, "f13" );
	bindKey( SDLK_F14, "f14" );
	bindKey( SDLK_F15, "f15" );

	bindKey( SDLK_NUMLOCK, "numlock" );
	bindKey( SDLK_CAPSLOCK, "capslock" );
	bindKey( SDLK_SCROLLOCK, "scrollock" );
	bindKey( SDLK_RSHIFT, "rshift" );
	bindKey( SDLK_LSHIFT, "lshift" );
	bindKey( SDLK_RCTRL, "rctrl" );
	bindKey( SDLK_LCTRL, "lctrl" );
	bindKey( SDLK_RALT, "ralt" );
	bindKey( SDLK_LALT, "lalt" );
	bindKey( SDLK_RMETA, "rmeta" );
	bindKey( SDLK_LMETA, "lmeta" );
	bindKey( SDLK_LSUPER, "lwin" );
	bindKey( SDLK_RSUPER, "rwin" );
	// SDLK_MODE		= 313,		/**< "Alt Gr" key */
	// SDLK_COMPOSE		= 314,		/**< Multi-key compose key */

	// SDLK_HELP		= 315,
	bindKey( SDLK_PRINT, "printscreen" );
	bindKey( SDLK_SYSREQ, "sysreq" );
	bindKey( SDLK_BREAK, "break" );
	bindKey( SDLK_MENU, "menu" );
	// SDLK_POWER		= 320,		/**< Power Macintosh power key */
	// SDLK_EURO		= 321,		/**< Some european keyboards */
	// SDLK_UNDO		= 322,		/**< Atari keyboard has Undo */

	bindKey( SDLK_LAST + 1, "joyaxis01" );
	bindKey( SDLK_LAST + 2, "joyaxis00" );
	bindKey( SDLK_LAST + 3, "joyaxis11" );
	bindKey( SDLK_LAST + 4, "joyaxis10" );
	bindKey( SDLK_LAST + 5, "joyaxis21" );
	bindKey( SDLK_LAST + 6, "joyaxis20" );
	bindKey( SDLK_LAST + 7, "joy1" );
	bindKey( SDLK_LAST + 8, "joy2" );
	bindKey( SDLK_LAST + 9, "joy3" );
	bindKey( SDLK_LAST + 10, "joy4" );
	bindKey( SDLK_LAST + 11, "joy5" );
	bindKey( SDLK_LAST + 12, "joy6" );
	bindKey( SDLK_LAST + 13, "joy7" );
	bindKey( SDLK_LAST + 14, "joy8" );
	bindKey( SDLK_LAST + 15, "joy9" );
	bindKey( SDLK_LAST + 16, "joy10" );
	bindKey( SDLK_LAST + 17, "joy11" );
	bindKey( SDLK_LAST + 18, "joy12" );
	bindKey( SDLK_LAST + 19, "joy13" );

	//memset( &BindedFunctions[0], 0, sizeof(Bindings) );
}


void Bindings::bindKey( int key, const char* name )
{
	if( key > MAXKEYS )
		return;
	KeyNames[key] = name;
}


const char* Bindings::getCurrent( )
{
	return Current;
}


const char* Bindings::keyName( UINT i )
{
	if( i >= MAXKEYS || KeyNames[i] == NULL )
		return none_string;
	return KeyNames[i];
}


void Bindings::process( int num, short down, UINT16 unicode )
{
	switch(BindedFunctions[num].type){
		case CFUNC:
			BindedFunctions[num].cref(down);
			break;
		case LUAFUNC:
			if( down )
				luaScript->ExecChunkFromReg( BindedFunctions[num].luaref );
			break;
		case NOTAFUNC:
			if( Reciever != LUA_NOREF ){
				luaScript->push( num );
				luaScript->push( down );
				wchar_t w[2] = { unicode, '\0' };
				luaScript->push( std::wstring(w) );
				luaScript->ExecChunkFromReg( Reciever, 3 );
			}
			break;
	}
}


void Bindings::bindCFunction( int key, UINT funcnumber )
{
	if( key < 0 || key > MAXKEYS )
		return;
	BindedFunctions[key].type = (func_t)Binds::getFunction( (Binds::func_numbers)funcnumber, &BindedFunctions[key].cref );
}


void Bindings::bindLuaFunction( int key, LuaRegRef func )
{
	if( key < 0 || key > MAXKEYS )
		return;
	BindFunction& bfunct = BindedFunctions[key];
	bfunct.type = LUAFUNC;
	if( bfunct.luaref != func ){
		luaScript->ReleaseObject( &bfunct.luaref );
		bfunct.luaref = func;
	}
}


LuaRegRef& Bindings::getReciever( ) {
	return Reciever;
}


#define BIND_TO_KEY( bindarr, bindfinc )												\
	FOREACHIT( bindarr ){																\
		if( it->first < MAXKEYS ){														\
			bindfinc( it->first, it->second );											\
			BindedKeys.push_back( it->first );											\
			std::map < UINT, UINT >::iterator alias = BindAliases.find( it->first );	\
			if( alias != BindAliases.end() ){											\
				bindfinc( alias->second, it->second );									\
				BindedKeys.push_back( alias->second );									\
			}																			\
		}else																			\
			Debug::debug( Debug::INPUT, "Bad key name.\n" );							\
	}


void Bindings::loadKeys( const char* subconfig )
{
	Debug::debug( Debug::INPUT, "Loading bindings set %s.\n", subconfig );
	freeKeys();
	Current = subconfig;
	BindedKeys.clear();
	std::map < UINT, UINT > Bindkeys;
	std::map < UINT, LuaRegRef > Bindfuncs;
	std::map < UINT, UINT > BindAliases;
	const char* config = "bindings";
	LuaScriptConfig* cfg = new LuaScriptConfig;
	cfg->getValue( "keys", subconfig, config, Bindkeys, Bindfuncs );
	cfg->LuaConfig::getValue( "aliases", subconfig, config, BindAliases );

	delete cfg;

	BIND_TO_KEY( Bindkeys, bindCFunction )
	BIND_TO_KEY( Bindfuncs, bindLuaFunction )

}

#undef BIND_TO_KEY


void Bindings::freeKeys( )
{
	ITER_LIST( UINT, BindedKeys )
		BindedFunctions[it->data].type = NOTAFUNC;
}
