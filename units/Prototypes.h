/*
 * Actions.h
 *
 *  Created on: 27.04.2012
 *
 */

#ifndef ACTIONS_H_
#define ACTIONS_H_

#include "types.h"
#include "ProtoStack.h"
#include <string>
#include <map>

enum ActionCommand
{
	acNone,
	acPushInt, acPushFloat, acPushString,
	acSetParam, acCopyParam, acLoadParam, acLoadParamBunch,
};

struct Frame
{
	UINT duration;
	UINT num;

	int param;
	char* txt_param;
	ActionCommand command;
	bool is_param_function;

	Frame( ) : duration(), num(), param(), command(acNone), is_param_function(false) {};
};

struct Action
{
	std::string name;
	size_t framesCount;
	Frame* frames;
};

struct Proto
{
	int id;
	std::string name;
	std::string parent;

	std::map< std::string, Action > Actions;

	Proto() : id(-1) {};
};


struct ActionManager
{
	bool loaded;
	bool done;
	UINT lastTick;
	UINT frame;
	Proto proto;
	ParametersStack params;
	Action* action;

	ActionManager( ) : loaded(false), done(false), lastTick(0), frame(0) { action = NULL; }
	~ActionManager( ) {}

	void setProto( Proto );
	void setAction( std::string );

	bool nextFrame( );

};


#endif /* ACTIONS_H_ */
