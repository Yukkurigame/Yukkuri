#ifndef YUKKURI_H
#define YUKKURI_H

#include "engine.h"
#include "unitmanager.h"
#include "daytime.h"
#include "Spawner.h"


class Yukkuri: public CEngine
{
public:
	bool Init( );
	bool AdditionalInit( );

	void Think( const int& ElapsedTime );
	void Render( );

	void MouseMoved( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );
	void MouseButtonUp( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );
	void MouseButtonDown( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );

	void WindowInactive( );
	void WindowActive( );

	void End();

private:
	UnitManager* units;
	DayTime daytime;
	Spawner spawner;
};

#endif //YUKKURI_H
