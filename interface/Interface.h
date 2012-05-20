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

	void clean();

	void LoadAllWidgets();
	Widget* LoadWidget( std::string name );

	Widget* GetWidget( unsigned int id );
	Widget* GetWidget( std::string name, Widget* parent );

	void Update( );

};

#endif /* INTERFACE_H_ */
