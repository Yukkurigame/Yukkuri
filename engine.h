#ifndef ENGINE_H
#define ENGINE_H

//На самом деле я не хочу сюда лазить, здесь грязно и пакостно.

#include "Define.h"
#include "safestring.h"
using std::string;

#include "Graphics.h"
#include "Bindings.h"
#include "debug.h"

/** The base engine class. **/
class CEngine
{
private:

	/** Last iteration's tick value **/
	long Tick;
	long LastTick;

	/** Has quit been called? **/
	bool EndLoop;

	/** The title of the window **/
	const char* Title;

	/** Is the window minimized? **/
	bool Minimized;

	/** Variables to calculate the frame rate **/
	/** Tick counter. **/
	int FPSTickCounter;

	/** Frame rate counter. **/
	int FPSCounter;

	/** Stores the last calculated frame rate. **/
	float CurrentFPS;

	/** FPS Text **/
	char FPStext[10];

#ifdef JOYSTICKENABLE
	/** SDL joystick **/
	SDL_Joystick* joystick;
#endif

protected:
	void DoThink( const int& iElapsedTicks );
	void DoRender();

	void SetSize();

	void HandleInput();

public:
	CEngine();
	virtual ~CEngine();

	virtual bool Init();

	/** OVERLOADED - Data that should be initialized when the application starts. **/
	virtual bool AdditionalInit( ) { return false; }

	void Start();
	void Quit();

	/** OVERLOADED - All the games calculation and updating.
		@param iElapsedTime The time in milliseconds elapsed since the function was called last.
	**/
	virtual void Think( const int& ElapsedTime ) {};

	/** OVERLOADED - All the games rendering.
		@param pDestSurface The main screen surface.
	**/
	virtual void Render( ) {};

	/** OVERLOADED - Allocated data that should be cleaned up. **/
	virtual void End( ) {};

	/** OVERLOADED - Window is active again. **/
	virtual void WindowActive( ) {};

	/** OVERLOADED - Window is inactive. **/
	virtual void WindowInactive( ) {};

	/** OVERLOADED - Keyboard key has been released.
		@param KeyEnum The key number.
	**/
	virtual void KeyUp( const int& KeyEnum ) {};

	/** OVERLOADED - Keyboard key has been pressed.
		@param KeyEnum The key number.
	**/
	virtual void KeyDown( const int& KeyEnum ) {};

	/** OVERLOADED - The mouse has been moved.
		@param Button	Specifies if a mouse button is pressed.
		@param X		The mouse position on the X-axis in pixels.
		@param Y		The mouse position on the Y-axis in pixels.
		@param RelX		The mouse position on the X-axis relative to the last position, in pixels.
		@param RelY		The mouse position on the Y-axis relative to the last position, in pixels.
		@bug The Button variable is always NULL.
	**/
	virtual void MouseMoved( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY ) {};

	/** OVERLOADED - A mouse button has been released.
		@param Button	Specifies if a mouse button is pressed.
		@param X		The mouse position on the X-axis in pixels.
		@param Y		The mouse position on the Y-axis in pixels.
		@param RelX		The mouse position on the X-axis relative to the last position, in pixels.
		@param RelY		The mouse position on the Y-axis relative to the last position, in pixels.
	**/
	virtual void MouseButtonUp( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY ) {};

	/** OVERLOADED - A mouse button has been pressed.
		@param Button	Specifies if a mouse button is pressed.
		@param X		The mouse position on the X-axis in pixels.
		@param Y		The mouse position on the Y-axis in pixels.
		@param RelX		The mouse position on the X-axis relative to the last position, in pixels.
		@param RelY		The mouse position on the Y-axis relative to the last position, in pixels.
	**/
	virtual void MouseButtonDown( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY ) {};

	void SetTitle( const char* czTitle );
	const char*	 GetTitle();

	// Screen Core
	char* GetFPSText();
	float GetFPS();
	int getScreenW();
	int getScreenH();

};

#endif // ENGINE_H
