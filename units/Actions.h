/*
 * Actions.h
 *
 *  Created on: 27.04.2012
 *
 */

#ifndef ACTIONS_H_
#define ACTIONS_H_

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


#endif /* ACTIONS_H_ */
