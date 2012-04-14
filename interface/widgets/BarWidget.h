/*
 * BarWidget.h
 *
 *  Created on: 24.03.2012
 *
 */

#ifndef BARWIDGET_H_
#define BARWIDGET_H_

#include "widgets/TextWidget.h"

class BarWidget: public TextWidget
{
public:
	BarWidget( );
	~BarWidget( );

	bool load( std::string id );

	void createBar( std::string name, int picture, int height, color4u clr );
	void setBarValue( float val );
	void setBarSize( float val );

	void updatePosition( );

	bool bindBarMaxValue( float* val );

	void Update();

	void toggleVisibility( );

protected:
	Sprite* BarSprite;
	Sprite* TopSprite;

private:
	float BarX, BarY;
	float BarValue;
	float BarMaxValue; // in units
	float BarWidth; // in pixels

	float* BindedMaxValue;
};


#endif /* BARWIDGET_H_ */
