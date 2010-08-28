#ifndef TIMEOFDAY_H
#define TIMEOFDAY_H

#include <math.h>
#include "Graphics.h"
#include "Interface.h"	//FUUUU
#include "Widgets.h"		//UUUUU
#include "Define.h"

class DayTime
{
public:
	DayTime();
	~DayTime();
	void update( const int& dt );
	void draw( );
	int getDay( ){ return Day; }
	void loadInterface();

private:
	Widget*	text;

	int Day;
	float time;
	bool Updated;
	Sprite* sfield;
	int dark, ldark;
	//TODO: Можно делать красивые штуки, как в питоновской версии, вот только нужно ли?
	//Каждому углу свою прозрачность и свой цвет.
};

#endif //TIMEOFDAY_H
