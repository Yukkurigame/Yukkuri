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
#include <stack>
#include <map>

enum ActionCommand
{
	acNone,
	acPushInt, acPushFloat, acPushString,
	acSuper,
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

	Action( ) : framesCount(0), frames(NULL) {}
	~Action( );
};

struct Proto
{
	int id;
	std::string name;
	Proto* parent;

	std::map< std::string, Action > Actions;

	Proto() : id(-1) { parent = NULL; };

	Action* getAction( std::string );
	Action* getParentAction( std::string );
};

struct ActionManagerState
{
	UINT currentFrame;
	std::string currentAction;
	UINT prevActionTickDiff;

	ActionManagerState( UINT frame, std::string action, UINT diff ):
		currentFrame( frame ), currentAction( action ), prevActionTickDiff( diff ) { }
};


struct ActionManager
{
	bool loaded;
	bool done;
	bool forceParent;
	UINT lastTick;
	int frame;
	Proto* proto;
	Action* action;
	std::string actionname;
	ParametersStack params;

	std::stack< ActionManagerState > stateStack;

	ActionManager( ) : loaded(false), done(false), forceParent(false),
			lastTick(0), frame(-1), action(NULL) { }
	~ActionManager( ) { }

	void setProto( Proto* );
	void setAction( std::string );
	void setParentAction( std::string name = "" );

	bool nextFrame( );

	void saveState( );
	void restoreState( );
	void popState( );

};


#endif /* ACTIONS_H_ */
