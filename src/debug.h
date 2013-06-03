/*
 * debug.h
 *
 *  Created on: 29.06.2010
 */

#ifndef YUKKURIDEBUG_H_
#define YUKKURIDEBUG_H_

#define DEBUG
#define DEBUG_SHADERS
//#define DEBUGLEVEL 9

#define NAMED_DBG_MESSAGE


namespace Debug
{
	enum dbg_level { NONE=0, MAIN, OS, CONFIG, GRAPHICS,
		INPUT, SCRIPT, PROTO, UNIT, INTERFACE, MAP, LAST
	};

	void debug( enum dbg_level level, const char* format, ... );
}


#endif /* YUKKURIDEBUG_H_ */
