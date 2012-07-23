/*
 * ProtoStack.cpp
 *
 *  Created on: 01.05.2012
 *
 */

#include "ProtoStack.h"
#include <stdarg.h>
#include <cstring>

#include "safestring.h"
#include "debug.h"



StackElement::StackElement()
{
	next = NULL;
	type = stInt;
	data.intData = 0;
}

StackElement::~StackElement()
{
	if ( this->type == stString )
		delete[] this->data.stringData, this->data.stringData = NULL;
}



ParametersStack::ParametersStack()
{
	top = NULL;
	locked = false;
}

ParametersStack::~ParametersStack()
{
	locked = true;
	while( top ){
		StackElement* next = top->next;
		delete top, top = NULL;
		top = next;
	}
}


bool ParametersStack::CheckParamTypes( int num, enum StackElementType first, ... )
{
	va_list mark;
	StackElementType setp = first;
	StackElement* se = top;
	va_start( mark, first );
	for( int i = 0; i < num; i++ ){
		if( !se && setp && setp != stNone && setp != stIntOrNone ){
			Debug::debug( Debug::PROTO, "Wrong set of parameters in stack for the action command.\n" );
			return false;
		}

		if( se && se->type != setp && !( se->type == stInt && setp == stIntOrNone ) ){
			Debug::debug( Debug::PROTO, "Wrong set of parameters in stack for the action command.\n" );
			return false;
		}
		// TODO: warning: ‘StackElementType’ is promoted to ‘int’ when passed through ‘...’
		// note: (so you should pass ‘int’ not ‘StackElementType’ to ‘va_arg’)
		// note: if this code is reached, the program will abort
		setp = (StackElementType)va_arg( mark, int );
		if( se ) se = se->next;
	}
	va_end( mark );
	return true;
}

bool ParametersStack::isEmpty()
{
	if( this->top )
		return false;
	else
		return true;
}

int ParametersStack::PopInt()
{
	StackElement* sd = this->Pop();
	int ret = sd->data.intData;
	delete sd, sd = NULL;
	return ret;
}

int ParametersStack::GetInt( int depth )
{
	if( !this->top )
		return 0;
	StackElement* sd = this->top;
	for( int i = 0; i < depth; i++ ){
		sd = sd->next;
		if( !sd )
			return 0;
	}
	return sd->data.intData;
}

std::string ParametersStack::PopString( )
{
	StackElement* sd = this->Pop();
	std::string ret = sd->data.stringData;
	delete sd, sd = NULL;
	return ret;
}

const char* ParametersStack::GetString( int depth )
{
	if( !this->top )
		return 0;
	StackElement* sd = this->top;
	for( int i = 0; i < depth; i++ ){
		sd = sd->next;
		if ( !sd )
			return 0;
	}
	return sd->data.stringData;
}

void ParametersStack::Push( const char* str )
{
	if(this->locked)
		return;
	StackElement* se = new StackElement();
	se->data.stringData = strdup( str );
	se->type = stString;
	se->next = this->top;
	this->top = se;
}

void ParametersStack::Push( int num )
{
	if(this->locked)
		return;
	StackElement* se = new StackElement();
	se->data.intData = num;
	se->type = stInt;
	se->next = this->top;
	this->top = se;
}

StackElement* ParametersStack::Pop()
{
	if(!top){
		StackElement* se = new StackElement();
		se->data.intData = 0;
		se->next = NULL;
		se->type = stNone;
		return se;
	}else{
		StackElement* se = this->top;
		this->top = this->top->next;
		return se;
	}
}

void ParametersStack::Dump()
{
	Debug::debug( Debug::PROTO, "Stack Dump:\n");
	if (!this->top)
		return;
	StackElement* sd = this->top;
	int i = 0;
	int dsz = 50 + strlen(sd->data.stringData);
	char* dbg = new char[ dsz ];
	for( ; sd; sd = sd->next, i++){
		snprintf( dbg, dsz, "%d: t=%d, i=%d, s=%s\n", i, (int)sd->type, sd->data.intData,
					sd->type == stString ? sd->data.stringData : NULL );
		Debug::debug( Debug::PROTO, dbg );
	}
	delete dbg;
}
