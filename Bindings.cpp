/*
 * Bindings.cpp
 *
 *  Created on: 07.07.2010
 */

#include "Bindings.h"
#include "yukkuri.h"

Bindings Bindings::bnd;

//И здесь FUFUFU


Bindings::Bindings( )
{
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
}

void Bindings::BindKey( int key, string name )
{
	Keys[key] = name;
}

void Bindings::unBindKey( int key )
{
	Keys.erase(key);
}

void Bindings::process( int num, short down )
{
	//printf("%s, %s\n", Keys[num].c_str(), Binds[Keys[num]].c_str());
	if( Binds[Keys[num]] == "up" ){
		Binds::movePlayerUp(down);
	}else if( Binds[Keys[num]] == "down" ){
		Binds::movePlayerDown(down);
	}else if( Binds[Keys[num]] == "left" ){
		Binds::movePlayerLeft(down);
	}else if( Binds[Keys[num]] == "right" ){
		Binds::movePlayerRight(down);
	}else if( Binds[Keys[num]] == "exit" ){
			Binds::exit( engine );
	}else if( Binds[Keys[num]] == "screenshot" ){
			Binds::screenshot( );
	}
}

void Bindings::LoadKeys( )
{
	string subconfig = "game";
	string config = "bindings";
	LuaConfig::conf.getValue( "keys", subconfig, config, Binds );
}

void Binds::movePlayerUp( short down )
{
	UnitManager::units.GetPlayer()->moveUnit( 0, down * -1 );
}

void Binds::movePlayerDown( short down )
{
	UnitManager::units.GetPlayer()->moveUnit( 0, down );
}

void Binds::movePlayerLeft( short down )
{
	UnitManager::units.GetPlayer()->moveUnit( 1, down * -1 );
}

void Binds::movePlayerRight( short down )
{
	UnitManager::units.GetPlayer()->moveUnit( 1, down );
}

void Binds::screenshot( )
{
	Graphics::graph.SaveScreenshot( "screenshot" );
}

void Binds::exit( const void* e )
{
	((Yukkuri*)e)->Quit();
}
