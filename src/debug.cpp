/*
 * debug.cpp
 *
 *  Created on: 04.02.2013
 *
 */

#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#ifdef NAMED_DBG_MESSAGE
	#define MAX_DBG_LABEL 9
	static const char* dbg_names[Debug::LAST] = {
		"None", "Main", "Os", "Config", "Graphics", "Input",
		"Script", "Proto", "Units", "Interface", "Map"
	};
#else
	#define MAX_DBG_LABEL 2
#endif

static char debug_buf[512];


void Debug::debug( enum dbg_level level, const char* format, ... )
{
#ifdef DEBUG
	va_list ap;
	va_start( ap, format );
	vsprintf( debug_buf, format, ap );
	//vprintf(Format, ap);
	va_end( ap );

	if( level > NONE && level < LAST ){

#ifdef NAMED_DBG_MESSAGE
		printf("%s: %*s", dbg_names[level],
				(int)(MAX_DBG_LABEL - strlen(dbg_names[level]) + strlen(debug_buf)), debug_buf);
#else
		printf("%*d: %s", MAX_DBG_LABEL, level, debug_buf);
#endif

	}else{
		printf("  %*s", (int)(MAX_DBG_LABEL + strlen(debug_buf)), debug_buf);
	}
#endif
};
