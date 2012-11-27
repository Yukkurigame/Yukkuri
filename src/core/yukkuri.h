#ifndef YUKKURI_H
#define YUKKURI_H

enum GameState { gsNone, gsLoading, gsRunning, gsPaused, gsEnd };

namespace Yukkuri
{
	bool Init( );
	void Clean( );

	bool InitGraphics( );
	void CleanGraphics( );

	bool InitInput( );
	bool AdditionalInit( );

	/** All the games calculation and updating.
		@param iElapsedTime The time in milliseconds elapsed since the function was called last.
	**/
	void Think( const int& ElapsedTime );
	/** All the games rendering. **/
	void Render( );
	/** Handles all controller inputs. **/
	void HandleInput();

	void MouseMoved( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );
	void MouseButtonUp( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );
	void MouseButtonDown( const int& Button, const int& X, const int& Y, const int& RelX, const int& RelY );

	/** Window is inactive. **/
	void WindowInactive( );
	/** Window is active again. **/
	void WindowActive( );

	/** The main loop. **/
	void Start();
	void End();

	/* Set window title */
	void SetTitle( const char* czTitle );
	/* Get window title */
	const char*	 GetTitle();

	// Screen Core
	float getFPS();
	int getScreenW();
	int getScreenH();

	// Parameters
	static struct YGame {
		int width;
		int height;
		bool minimized;
		const char* title;
		enum GameState state;
	} Window;
};

#endif //YUKKURI_H
