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

	void createBar( std::string name, int picture, s4ub color );
	void setBarValue( float val );

	float getBarSize( ) { return BarMaxValue; }
	void  setBarSize( float val );


	void updatePosition( );

	bool bindBarMaxValue( float* val );
	bool bindBarMaxValue( lua_State* L );

	void Update();

	void toggleVisibility( );

	// Lua methods
	virtual bool load( lua_State* L );

protected:
	virtual CUData* createUData();

private:
	rect2f Bar; // Position & size ( in pixels ) of bar
	rect2f Top; // Position & size of top image
	float BarValue;
	float BarMaxValue; // in units

	Sprite* BarSprite;
	Sprite* TopSprite;

	float* BindedMaxValue;
};


#endif /* BARWIDGET_H_ */
