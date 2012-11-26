/*
 * debug.h
 *
 *  Created on: 29.06.2010
 */

#ifndef YUKKURIDEBUG_H_
#define YUKKURIDEBUG_H_

#define DEBUG
//#define DEBUG_DRAW_RECTANGLES 1
//#define DEBUG_DRAW_ATLAS_RECTANGLES
//#define DEBUGLEVEL 9

#define NAMED_DBG_MESSAGE


#include <stdio.h>
#include <string>
#include <cstring>

namespace Debug
{
	enum dbg_level { NONE=0, MAIN, OS, CONFIG, GRAPHICS, INPUT, SCRIPT, PROTO, UNIT, INTERFACE, MAP, LAST };
#ifdef NAMED_DBG_MESSAGE
	#define MAX_DBG_LABEL 9
	static const char* dbg_names[LAST] = { "None", "Main", "Os", "Config", "Graphics",
			"Input", "Script", "Proto", "Units", "Interface", "Map" };
#else
	#define MAX_DBG_LABEL 2
#endif

	//TODO: string to char*
	inline void debug( enum dbg_level level, std::string txt )
	{
	#ifdef DEBUG
		if( level > NONE && level < LAST ){
#ifdef NAMED_DBG_MESSAGE
				printf("%s: %*s", dbg_names[level], (MAX_DBG_LABEL - strlen(dbg_names[level])) + txt.length(), txt.c_str());
#else
				printf("%*d: %s", MAX_DBG_LABEL, level, txt.c_str());
#endif
		}else{
			printf("%*s", MAX_DBG_LABEL + txt.length(),  txt.c_str());
		}
	#endif
	};
}


#endif /* YUKKURIDEBUG_H_ */
