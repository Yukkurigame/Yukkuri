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

#define FRAME_PARAMS_COUNT 4

enum ActionCommand
{
	acNone = 0,
	acPush, acPushInt, acPushFloat, acPushString,
	acSuper, acRepeatDelay, acSetAction, acLoop,
	acCondition,
	// Comparison between parameter and number
	acIfParamEqual, acIfParamLess, acIfParamMore,
	// Comparison between parameter and parameter
	acIfParametersEqual, acIfParametersLess, acIfParametersMore, acIfParametersLessBy,
	acIfFlag, acIfNotFlag,
	acEnd,
	acSetParam, acCopyParam, acLoadParam, acLoadParamBunch,
	acSetFlag, acRemoveFlag,
	acMove
};

struct Frame
{
	UINT duration;
	UINT num;

	StackData params[FRAME_PARAMS_COUNT];
	StackElementType param_types[FRAME_PARAMS_COUNT];

	int condition_end;
	long repeat;
	LuaRegRef func;
	ActionCommand command;

	Frame( ) : duration(), num(), condition_end(), repeat(),
			func(-2), command(acNone) {};
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
	bool looped;
	UINT lastTick;
	int frame;
	Proto* proto;
	Action* action;
	std::string actionname;
	ParametersStack params;

	std::stack< ActionManagerState > stateStack;

	ActionManager( ) : loaded(false), done(false), forceParent(false), looped(false),
			lastTick(0), frame(-1), action(NULL) { }
	~ActionManager( ) { }

	void setProto( Proto* );
	void setAction( std::string, bool force = false );
	void setParentAction( const char* name );

	bool nextFrame( );

	void saveState( );
	void restoreState( );
	void popState( );

	// TODO: make anything with this
	bool checkFrameParams( const Frame& f, int num, enum StackElementType first, ... );

};


#endif /* ACTIONS_H_ */
