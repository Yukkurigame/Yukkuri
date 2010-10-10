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
#include "debug.h"


class UI
{
public:
	~UI();

	static UI yui;

	void LoadAllWidgets();
	Widget* LoadWidget( string name );

	Widget* GetWidget( unsigned int id );
	Widget* GetWidget( string name );

	void Update( );


private:

	void DeleteWidget( string name );

	std::vector< Widget* > widgets;

};

#endif /* INTERFACE_H_ */
