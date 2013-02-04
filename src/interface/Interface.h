/*
 * Interface.h
 *
 *  Created on: 30.06.2010
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "types.h"

class Widget;


namespace Interface
{
	void init( );
	void clean();

	Widget* CreateWidget( const char* id, UINT type );

	//void LoadAllWidgets();
	//Widget* LoadWidget( const char* id );

	Widget* GetWidget( UINT id );
	Widget* GetWidget( const char* name, Widget* parent );


};

#endif /* INTERFACE_H_ */
