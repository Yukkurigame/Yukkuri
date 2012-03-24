/*
 * Interface.h
 *
 *  Created on: 30.06.2010
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include "widgets/Widget.h"


class UI
{
public:
	~UI();

	static UI yui;

	void LoadAllWidgets();
	Widget* LoadWidget( std::string name );

	Widget* GetWidget( unsigned int id );
	Widget* GetWidget( std::string name );

	void Update( );


private:

	void DeleteWidget( std::string name );

	std::vector< Widget* > widgets;

};

#endif /* INTERFACE_H_ */
