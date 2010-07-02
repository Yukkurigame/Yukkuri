/*
 * debug.h
 *
 *  Created on: 29.06.2010
 */

#ifndef YUKKURIDEBUG_H_
#define YUKKURIDEBUG_H_

#define DEBUG

#define DEBUGLEVEL 9

//FIXME: headers!!!
#include <stdio.h>
#include <string>

inline void debug( int level, std::string txt )
{
#ifdef DEBUG
	if( level <= DEBUGLEVEL )
		printf("%d: %s", level, txt.c_str());
#endif
};


#endif /* YUKKURIDEBUG_H_ */
