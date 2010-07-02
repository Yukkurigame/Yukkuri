/*
 * Interface.h
 *
 *  Created on: 30.06.2010
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
using std::string;
#include <map>

#include "Widgets.h"
#include "Luaconfig.h"
#include "debug.h"


class UI
{
public:
	~UI();

	static UI yui;

	void LoadAllWidgets();
	Widget* LoadWidget( string name );

	Widget* GetWidget( string name );

	void DrawWidgets();

private:

	void DeleteWidget( string name );

	std::map< string, Widget*> widgets;

};

#endif /* INTERFACE_H_ */
