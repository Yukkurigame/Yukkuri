/*
 * utf.h
 *
 *  Created on: 24.06.2012
 *
 */

#ifndef UTF_H_
#define UTF_H_

#include <string>

wchar_t* str_to_UTF8( const char *s );
char* UTF8_to_str( const std::wstring &s );

#endif /* UTF_H_ */
