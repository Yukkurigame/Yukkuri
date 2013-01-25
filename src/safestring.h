/*
 * safestring.h
 *
 *  Created on: 24.03.2011
 */

#ifndef SAFESTRING_H_
#define SAFESTRING_H_

#include <string>
#include <cstring>
#include <sstream>
#include "types.h"

//TODO: strcat, strcpy

#if WIN32
	// TODO: sprintf_s и snprintf немного отличаются поведением.
	// sprintf_s гарантирует, что в конце буефера будет 0.
	#define snprintf sprintf_s
	#define strdup _strdup
#endif


// FIXME: slow?
inline std::string citoa( int i )
{
	std::stringstream out;
	out << i;
	return out.str();
}

#endif /* SAFESTRING_H_ */
