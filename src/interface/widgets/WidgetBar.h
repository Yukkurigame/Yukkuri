/*
 * BarWidget.h
 *
 *  Created on: 24.03.2012
 *
 */

#ifndef BARWIDGET_H_
#define BARWIDGET_H_

#include "interface/widgets/WidgetText.h"

class WidgetBar: public WidgetText
{
public:
	WidgetBar( );
	~WidgetBar( );

	bool load( std::string id );

	void createBar( std::string name, int picture, int height, s4ub color );
	void setBarValue( float val );

	float getBarSize( ) { return BarMaxValue; }
	void  setBarSize( float val );


	void updatePosition( );

	bool bindBarMaxValue( float* val );
	bool bindBarMaxValue( lua_State* L );

	void Update();

	void toggleVisibility( );

protected:
	virtual CUData* createUData();

	Sprite* BarSprite;
	Sprite* TopSprite;

private:
	float BarX, BarY, TopX, TopY;
	float BarValue;
	float BarMaxValue; // in units
	float BarWidth; // in pixels

	float* BindedMaxValue;
};


#endif /* BARWIDGET_H_ */
