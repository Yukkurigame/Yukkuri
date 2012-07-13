/*
 * ProtoStack.h
 *
 *  Created on: 01.05.2012
 *
 */

#ifndef PROTOSTACK_H_
#define PROTOSTACK_H_

#include <string>

// Данные хранящиеся в стеке.
// В одном элементе стека может быть целое число или строка.
typedef union
{
	int intData;
	const char* stringData;
} StackData;

enum StackElementType
{
	stNone, stInt, stString, stIntOrNone, stStringOrNone,
};

// Элемент стека
struct StackElement
{
	StackElement* next;
	StackData data;
	StackElementType type;

	StackElement();
	~StackElement();
};

struct ParametersStack
{
	StackElement* top;
	bool locked;

	ParametersStack();
	~ParametersStack();


	bool CheckParamTypes(int num, enum StackElementType first, ...);
	bool isEmpty();
	int PopInt();
	int GetInt(int depth = 0);
	std::string PopString( );
	const char* GetString(int depth = 0);
	void Push(const char* str);
	void Push(int num);
	StackElement* Pop();
	void Dump();
};


#endif /* PROTOSTACK_H_ */
