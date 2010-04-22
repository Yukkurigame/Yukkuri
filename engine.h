#ifndef ENGINE_H
#define ENGINE_H

#include "Define.h" 
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;

#include "SDL/SDL.h" 
#include "SDL/SDL_ttf.h"

/** The base engine class. **/
class CEngine  
{
private:
    /** Last iteration's tick value **/
    long m_lTick;
    long m_lLastTick;
    
    /** Window width **/
    //int m_iWidth;
    /** Window height **/
    //int m_iHeight;
 
    /** Has quit been called? **/
    bool m_bQuit;
 
    /** The title of the window **/
    const char* m_czTitle;
 
    /** Screen surface **/
    SDL_Surface* m_pScreen;
 
    /** Is the window minimized? **/
    bool m_bMinimized;
 
	/** Variables to calculate the frame rate **/
	/** Tick counter. **/
	int m_iFPSTickCounter;

	/** Frame rate counter. **/
	int m_iFPSCounter;

	/** Stores the last calculated frame rate. **/
	int m_iCurrentFPS;
	
	/** FPS Surface **/
	SDL_Surface* m_pFPSDisp;
	
	/** FPS Font **/
	TTF_Font* m_pFPSFont;
	
	/** FPS Surface **/
	char* m_sFPStext;
	
	/** FPS Display Toggle **/
	bool m_bFPStoggle;

   /** SDL joystick **/	
	SDL_Joystick *joystick;
	
protected:
    void DoThink( const int& iElapsedTicks );
    void DoRender();
 
    void SetSize();
 
    void HandleInput();
    void DisplayFPS();
 
public:
    CEngine();
    virtual ~CEngine();
 
    void Init();
    void Start();
 
    /** OVERLOADED - Data that should be initialized when the application starts. **/
    virtual void AdditionalInit    () {}
 
    /** OVERLOADED - All the games calculation and updating. 
        @param iElapsedTime The time in milliseconds elapsed since the function was called last.
    **/
    virtual void Think        ( const int& iElapsedTime ) {}
    /** OVERLOADED - All the games rendering. 
        @param pDestSurface The main screen surface.
    **/
    virtual void Render        ( SDL_Surface* pDestSurface ) {}
 
    /** OVERLOADED - Allocated data that should be cleaned up. **/
    virtual void End        () {}
 
    /** OVERLOADED - Window is active again. **/
    virtual void WindowActive    () {}
 
    /** OVERLOADED - Window is inactive. **/
    virtual void WindowInactive    () {}
 
 
    /** OVERLOADED - Keyboard key has been released.
        @param iKeyEnum The key number.
    **/
    virtual void KeyUp        (const int& iKeyEnum) {}
 
    /** OVERLOADED - Keyboard key has been pressed.
        @param iKeyEnum The key number.
    **/
    virtual void KeyDown        (const int& iKeyEnum) {}
 
 
    /** OVERLOADED - The mouse has been moved.
        @param iButton    Specifies if a mouse button is pressed.
        @param iX    The mouse position on the X-axis in pixels.
        @param iY    The mouse position on the Y-axis in pixels.
        @param iRelX    The mouse position on the X-axis relative to the last position, in pixels.
        @param iRelY    The mouse position on the Y-axis relative to the last position, in pixels.
 
        @bug The iButton variable is always NULL.
    **/
    virtual void MouseMoved        (const int& iButton,
                     const int& iX, 
                     const int& iY, 
                     const int& iRelX, 
                     const int& iRelY) {}
 
    /** OVERLOADED - A mouse button has been released.
        @param iButton    Specifies if a mouse button is pressed.
        @param iX    The mouse position on the X-axis in pixels.
        @param iY    The mouse position on the Y-axis in pixels.
        @param iRelX    The mouse position on the X-axis relative to the last position, in pixels.
        @param iRelY    The mouse position on the Y-axis relative to the last position, in pixels.
    **/
 
    virtual void MouseButtonUp    (const int& iButton, 
                     const int& iX, 
                     const int& iY, 
                     const int& iRelX, 
                     const int& iRelY) {}
 
    /** OVERLOADED - A mouse button has been pressed.
        @param iButton    Specifies if a mouse button is pressed.
        @param iX    The mouse position on the X-axis in pixels.
        @param iY    The mouse position on the Y-axis in pixels.
        @param iRelX    The mouse position on the X-axis relative to the last position, in pixels.
        @param iRelY    The mouse position on the Y-axis relative to the last position, in pixels.
    **/
    virtual void MouseButtonDown    (const int& iButton, 
                     const int& iX, 
                     const int& iY, 
                     const int& iRelX, 
                     const int& iRelY) {}
 
    void        SetTitle( const char* czTitle );
    const char*     GetTitle();

    // Graphics Core    SDL_Surface* GetSurface();
    //SDL_Rect GetTileClip( SDL_Surface*, int, int );

    // Screen Core         int GetFPS();
    int getScreenW() { return WWIDTH; }
    int getScreenH() { return WHEIGHT; }

    /** Player move **/
    signed int player_movex;
    signed int player_movey;
     
};
 
#endif // ENGINE_H 