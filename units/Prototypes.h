/*
 * Actions.h
 *
 *  Created on: 27.04.2012
 *
 */

#ifndef ACTIONS_H_
#define ACTIONS_H_

#include "types.h"
#include <string>
#include <map>

enum ActionCommand
{
	acNone
};

struct Frame
{
	UINT duration;
	UINT num;

	int param;
	char* txt_param;
	ActionCommand command;
	bool is_param_function;

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
	Action* action;

	ActionManager( ) : loaded(false), frame(-1) {}
	~ActionManager( ) {}

	void setProto( Proto );
	void setAction( std::string );

	bool nextFrame( );

};


#endif /* ACTIONS_H_ */
