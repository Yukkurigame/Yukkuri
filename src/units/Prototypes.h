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
	acSuper, acRestoreState, acSetAction, acSetTimer,
	acCondition,
	// Comparison between parameter and number
	acIfParamEqual, acIfParamLess, acIfParamMore,
	// Comparison between parameter and parameter
	acIfParametersEqual, acIfParametersLess, acIfParametersMore, acIfParametersLessBy,
	acIfFlag, acIfNotFlag,
	acEnd,
	acSetParam, acCopyParam, acLoadParam, acLoadParamBunch,
	acSetFlag, acRemoveFlag,
	// Unit
	acSetUnitSize, acSetColor,
	// Unit physics
	acSetUnitPhysics, acSetPhysicsLayer,
	// Dynamic unit
	acMove, acDAddFood, acDUnitGrow,
	// Entity
	acEAddPathTarget,
};

struct Frame
{
	UINT duration;
	UINT num;

	StackData params[FRAME_PARAMS_COUNT];
	StackElementType param_types[FRAME_PARAMS_COUNT];

	int condition_end;
	int skip_on_delay;
	LuaRegRef func;
	ActionCommand command;

	Frame( ) : duration(), num(), condition_end(), skip_on_delay(),
			func(-2), command(acNone) {};
};

struct FrameTimer {
	Frame* frame;
	int time;
	FrameTimer* next;
	FrameTimer() : frame(NULL), time(), next(NULL) {}
	FrameTimer( Frame* f, int t, FrameTimer* n ) : frame(f), time(t), next(n) {}
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

	int physicsType;
	bool statical;


	std::map< std::string, Action > Actions;

	Proto() : id(-1) { parent = NULL; };

	Action* getAction( std::string );
	Action* getParentAction( std::string );
};

struct ActionManagerState
{
	UINT currentFrame;
	Action* currentAction;
	Proto* currentPrototype;
	UINT prevActionTickDiff;
	bool forced;

	ActionManagerState( UINT frame, Action* action, Proto* proto, UINT diff, bool force ):
		currentFrame( frame ), currentAction( action ), currentPrototype( proto ),
		prevActionTickDiff( diff ), forced( force ) { }
};

struct ActionManager
{
	bool loaded;
	bool done;
	long lastTick;
	int frame;
	Proto* proto;
	Action* action;
	ParametersStack params;
	FrameTimer* skipTimers;

	std::stack< ActionManagerState* > stateStack;

	ActionManager( ) : loaded(false), done(false),
			lastTick(0), frame(-1), action(NULL), skipTimers(NULL) { }
	~ActionManager( );

	void setProto( Proto* );
	void setAction( const char* name );
	void setParentAction( const char* name );

	void updateTimers( const int& dt );
	bool nextFrame( );

	void saveState( bool forced );
	void restoreState( );
	void popState( );
	inline bool topStateForced( ){
		if( !stateStack.empty() )
			return stateStack.top()->forced;
		return false;
	}



	// TODO: make anything with this
	bool checkFrameParams( const Frame& f, int num, enum StackElementType first, ... );

};


#endif /* ACTIONS_H_ */
