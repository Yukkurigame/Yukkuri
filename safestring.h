/*
 * safestring.h
 *
 *  Created on: 24.03.2011
 */

#ifndef SAFESTRING_H_
#define SAFESTRING_H_

#include <string>

//TODO: strcat, strcpy
// TODO: sprintf_s и snprintf немного отличаются поведением.
// sprintf_s гарантирует, что в конце буефера будет 0.

#if WIN32
	#define snprintf sprintf_s
#endif



#endif /* SAFESTRING_H_ */
