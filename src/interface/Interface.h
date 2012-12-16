/*
 * Interface.h
 *
 *  Created on: 30.06.2010
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include "widgets/Widget.h"


namespace Interface
{
	void init( );
	void clean();

	Widget* CreateWidget( std::string id, enum wType type );

	void LoadAllWidgets();
	Widget* LoadWidget( std::string id );

	Widget* GetWidget( unsigned int id );
	Widget* GetWidget( std::string name, Widget* parent );

	//void Update( );

};

#endif /* INTERFACE_H_ */
