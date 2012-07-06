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
#include <cstring>
#include <cstdlib>

#include "hacks.h"

Bindings Bindings::bnd;
extern LuaScript* luaScript;


Bindings::Bindings( )
{
	Reciever = LUA_NOREF;

	BindKey( SDLK_BACKSPACE, "backspace" );
	BindKey( SDLK_TAB, "tab" );
	// SDLK_CLEAR = 12,
	BindKey( SDLK_RETURN, "enter" );
	BindKey( SDLK_PAUSE, "pause" );
	BindKey( SDLK_ESCAPE, "esc" );
	BindKey( SDLK_SPACE, "space" );
	BindKey( SDLK_EXCLAIM, "exclaim" );
	BindKey( SDLK_QUOTEDBL, "quotedbl" );
	BindKey( SDLK_HASH, "hash" );
	BindKey( SDLK_DOLLAR, "dollar" );
	BindKey( SDLK_AMPERSAND, "ampersand" );
	BindKey( SDLK_QUOTE, "quote" );
	BindKey( SDLK_LEFTPAREN, "leftparen" );
	BindKey( SDLK_RIGHTPAREN, "rightparen" );
	BindKey( SDLK_ASTERISK, "asterisk" );
	BindKey( SDLK_PLUS, "plus" );
	BindKey( SDLK_COMMA, "comma" );
	BindKey( SDLK_MINUS, "minus" );
	BindKey( SDLK_PERIOD, "period" );
	BindKey( SDLK_SLASH, "slash" );

	BindKey( SDLK_0, "0" );
	BindKey( SDLK_1, "1" );
	BindKey( SDLK_2, "2" );
	BindKey( SDLK_3, "3" );
	BindKey( SDLK_4, "4" );
	BindKey( SDLK_5, "5" );
	BindKey( SDLK_6, "6" );
	BindKey( SDLK_7, "7" );
	BindKey( SDLK_8, "8" );
	BindKey( SDLK_9, "9" );
	BindKey( SDLK_COLON, "colon" );
	BindKey( SDLK_SEMICOLON, "semicolon" );
	BindKey( SDLK_LESS, "less" );
	BindKey( SDLK_EQUALS, "equals" );
	BindKey( SDLK_GREATER, "greater" );
	BindKey( SDLK_QUESTION, "question" );
	BindKey( SDLK_AT, "at" );

	BindKey( SDLK_LEFTBRACKET, "lbracket" );
	BindKey( SDLK_BACKSLASH, "backslash" );
	BindKey( SDLK_RIGHTBRACKET, "rbracket" );
	BindKey( SDLK_CARET, "caret" );
	BindKey( SDLK_UNDERSCORE, "underscore" );
	BindKey( SDLK_BACKQUOTE, "backquote" );
	BindKey( SDLK_a, "a" );
	BindKey( SDLK_b, "b" );
	BindKey( SDLK_c, "c" );
	BindKey( SDLK_d, "d" );
	BindKey( SDLK_e, "e" );
	BindKey( SDLK_f, "f" );
	BindKey( SDLK_g, "g" );
	BindKey( SDLK_h, "h" );
	BindKey( SDLK_i, "i" );
	BindKey( SDLK_j, "j" );
	BindKey( SDLK_k, "k" );
	BindKey( SDLK_l, "l" );
	BindKey( SDLK_m, "m" );
	BindKey( SDLK_n, "n" );
	BindKey( SDLK_o, "o" );
	BindKey( SDLK_p, "p" );
	BindKey( SDLK_q, "q" );
	BindKey( SDLK_r, "r" );
	BindKey( SDLK_s, "s" );
	BindKey( SDLK_t, "t" );
	BindKey( SDLK_u, "u" );
	BindKey( SDLK_v, "v" );
	BindKey( SDLK_w, "w" );
	BindKey( SDLK_x, "x" );
	BindKey( SDLK_y, "y" );
	BindKey( SDLK_z, "z" );
	BindKey( SDLK_DELETE, "delete" );

	/* International keyboard syms later */

	BindKey( SDLK_KP0, "num0" );
	BindKey( SDLK_KP1, "num1" );
	BindKey( SDLK_KP2, "num2" );
	BindKey( SDLK_KP3, "num3" );
	BindKey( SDLK_KP4, "num4" );
	BindKey( SDLK_KP5, "num5" );
	BindKey( SDLK_KP6, "num6" );
	BindKey( SDLK_KP7, "num7" );
	BindKey( SDLK_KP8, "num8" );
	BindKey( SDLK_KP9, "num9" );
	BindKey( SDLK_KP_PERIOD, "decimal" );
	BindKey( SDLK_KP_DIVIDE, "divide" );
	BindKey( SDLK_KP_MULTIPLY, "multiply" );
	BindKey( SDLK_KP_MINUS, "kpminus" );
	BindKey( SDLK_KP_PLUS, "kpplus" );
	BindKey( SDLK_KP_ENTER, "kpenter" );
	BindKey( SDLK_KP_EQUALS, "equals" );

	BindKey( SDLK_UP, "up" );
	BindKey( SDLK_DOWN, "down" );
	BindKey( SDLK_RIGHT, "right" );
	BindKey( SDLK_LEFT, "left" );
	BindKey( SDLK_INSERT, "insert" );
	BindKey( SDLK_HOME, "home" );
	BindKey( SDLK_END, "end" );
	BindKey( SDLK_PAGEUP, "pgup" );
	BindKey( SDLK_PAGEDOWN, "pgdown" );

	BindKey( SDLK_F1, "f1" );
	BindKey( SDLK_F2, "f2" );
	BindKey( SDLK_F3, "f3" );
	BindKey( SDLK_F4, "f4" );
	BindKey( SDLK_F5, "f5" );
	BindKey( SDLK_F6, "f6" );
	BindKey( SDLK_F7, "f7" );
	BindKey( SDLK_F8, "f8" );
	BindKey( SDLK_F9, "f9" );
	BindKey( SDLK_F10, "f10" );
	BindKey( SDLK_F11, "f11" );
	BindKey( SDLK_F12, "f12" );
	BindKey( SDLK_F13, "f13" );
	BindKey( SDLK_F14, "f14" );
	BindKey( SDLK_F15, "f15" );

	BindKey( SDLK_NUMLOCK, "numlock" );
	BindKey( SDLK_CAPSLOCK, "capslock" );
	BindKey( SDLK_SCROLLOCK, "scrollock" );
	BindKey( SDLK_RSHIFT, "rshift" );
	BindKey( SDLK_LSHIFT, "lshift" );
	BindKey( SDLK_RCTRL, "rctrl" );
	BindKey( SDLK_LCTRL, "lctrl" );
	BindKey( SDLK_RALT, "ralt" );
	BindKey( SDLK_LALT, "lalt" );
	BindKey( SDLK_RMETA, "rmeta" );
	BindKey( SDLK_LMETA, "lmeta" );
	BindKey( SDLK_LSUPER, "lwin" );
	BindKey( SDLK_RSUPER, "rwin" );
	// SDLK_MODE		= 313,		/**< "Alt Gr" key */
	// SDLK_COMPOSE		= 314,		/**< Multi-key compose key */

	// SDLK_HELP		= 315,
	BindKey( SDLK_PRINT, "printscreen" );
	BindKey( SDLK_SYSREQ, "sysreq" );
	BindKey( SDLK_BREAK, "break" );
	BindKey( SDLK_MENU, "menu" );
	// SDLK_POWER		= 320,		/**< Power Macintosh power key */
	// SDLK_EURO		= 321,		/**< Some european keyboards */
	// SDLK_UNDO		= 322,		/**< Atari keyboard has Undo */

	BindKey( SDLK_LAST + 1, "joyaxis01" );
	BindKey( SDLK_LAST + 2, "joyaxis00" );
	BindKey( SDLK_LAST + 3, "joyaxis11" );
	BindKey( SDLK_LAST + 4, "joyaxis10" );
	BindKey( SDLK_LAST + 5, "joyaxis21" );
	BindKey( SDLK_LAST + 6, "joyaxis20" );
	BindKey( SDLK_LAST + 7, "joy1" );
	BindKey( SDLK_LAST + 8, "joy2" );
	BindKey( SDLK_LAST + 9, "joy3" );
	BindKey( SDLK_LAST + 10, "joy4" );
	BindKey( SDLK_LAST + 11, "joy5" );
	BindKey( SDLK_LAST + 12, "joy6" );
	BindKey( SDLK_LAST + 13, "joy7" );
	BindKey( SDLK_LAST + 14, "joy8" );
	BindKey( SDLK_LAST + 15, "joy9" );
	BindKey( SDLK_LAST + 16, "joy10" );
	BindKey( SDLK_LAST + 17, "joy11" );
	BindKey( SDLK_LAST + 18, "joy12" );
	BindKey( SDLK_LAST + 19, "joy13" );

	//memset( &BindedFunctions[0], 0, sizeof(Bindings) );
}

Bindings::~Bindings( )
{

}

void Bindings::BindKey( int key, std::string name )
{
	if( key > MAXKEYS )
		return;
	KeyNames[key] = name;
	//Keys[key] = key;
}

void Bindings::unBindKey( std::string name )
{
	//std::map< std::string, UINT >::iterator fkey = Keys.find( name );
	//if( fkey != Keys.end() ){
	//	KeyNames[fkey->second] = "";
	//	Keys.erase( fkey );
	//}
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


void Bindings::BindCFunction( int key, UINT funcnumber )
{
	if( key < 0 || key > MAXKEYS )
		return;
	BindedFunctions[key].type = (func_t)Binds::getFunction( (Binds::func_numbers)funcnumber, &BindedFunctions[key].cref );
}


void Bindings::BindLuaFunction( int key, LuaRegRef func )
{
	if( key < 0 || key > MAXKEYS )
		return;
	BindFunction& bfunct = BindedFunctions[key];
	bfunct.type = LUAFUNC;
	if( bfunct.luaref != func ){
		luaScript->ReleaseProc( &bfunct.luaref );
		bfunct.luaref = func;
	}
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

void Bindings::LoadKeys( std::string subconfig )
{
	Debug::debug( Debug::INPUT, "Loading bindings set " + subconfig + ".\n" );
	FreeKeys();
	Current = subconfig;
	BindedKeys.clear();
	std::map < UINT, UINT > Bindkeys;
	std::map < UINT, LuaRegRef > Bindfuncs;
	std::map < UINT, UINT > BindAliases;
	std::string config = "bindings";
	LuaScriptConfig* cfg = new LuaScriptConfig;
	cfg->getValue( "keys", subconfig, config, Bindkeys, Bindfuncs );
	cfg->LuaConfig::getValue( "aliases", subconfig, config, BindAliases );

	delete cfg;

	BIND_TO_KEY( Bindkeys, BindCFunction )
	BIND_TO_KEY( Bindfuncs, BindLuaFunction )

}

#undef BIND_TO_KEY

void Bindings::FreeKeys( )
{
	FOREACHIT( BindedKeys )
		BindedFunctions[*it].type = NOTAFUNC;
}
