/*
 * debug.h
 *
 *  Created on: 29.06.2010
 */

#ifndef YUKKURIDEBUG_H_
#define YUKKURIDEBUG_H_

#define DEBUG

#define DEBUGLEVEL 9

//#define DEBUG_DRAW_RECTANGLES

#include <stdio.h>
#include <string>

namespace Debug
{
	enum dbg_level { NONE, MAIN, OS, CONFIG, GRAPHICS, SCRIPT, INTERFACE, MAP };

	//TODO: string to char*
	inline void debug( enum dbg_level level, std::string txt )
	{
	#ifdef DEBUG
		if( level != NONE ){
			if( level <= DEBUGLEVEL )
				printf("%d: %s", level, txt.c_str());
		}else{
			printf("%s", txt.c_str());
		}
	#endif
	};
}


#endif /* YUKKURIDEBUG_H_ */
