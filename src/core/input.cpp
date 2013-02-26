/*
 * input.cpp
 *
 *  Created on: 27.11.2012
 */

#include "core/yukkuri.h"
#include "Define.h"
#include "debug.h"
#include "Bindings.h"

#include <string>
#include "SDL/SDL.h"
#include "graphics/Camera.h"



namespace {
	SDL_Joystick* joystick;
}


bool Yukkuri::InitInput( )
{
#ifdef JOYSTICKENABLE
	if( SDL_NumJoysticks() > 0 ){
		int jnum = SDL_NumJoysticks();
		debug( Debug::INPUT, "%d joysticks were found:\n", jnum );
		for( int i=0; i < jnum; i++ )
			debug( Debug::INPUT, "%s\n", SDL_JoystickName(i) );
		SDL_JoystickEventState(SDL_ENABLE);
		joystick = SDL_JoystickOpen(0);
	}
#else
	debug( Debug::INPUT, "Joystick not enabled.\n" );
#endif

	SDL_EnableUNICODE( 1 );

	return true;
}

/** Handles all controller inputs. **/
void Yukkuri::HandleInput()
{
	// Poll for events, and handle the ones we care about.
	SDL_Event event;

	while( SDL_PollEvent( &event ) )
	{
		int evnt = 0;
		Uint16 unicode = 0;
		short down = 1;
		switch( event.type ){

			case SDL_QUIT:
				End( );
				break;

			case SDL_ACTIVEEVENT:
				if( event.active.state & SDL_APPACTIVE ){
					if( event.active.gain ){
						Window.minimized = false;
						WindowActive( );
					}else{
						Window.minimized = true;
						WindowInactive( );
					}
				}
				break;

			case SDL_KEYUP:
				down = 0;	//Yes, no break.
			case SDL_KEYDOWN:
				evnt = event.key.keysym.sym;
				unicode = event.key.keysym.unicode;
				break;			//It's break for both SDL_KEY events.

#ifdef JOYSTICKENABLE
			case SDL_JOYAXISMOTION:
				evnt = SDLK_LAST;
				if( event.jaxis.axis == 0 ){ // Left-right movement
					//left, right, release
					if( event.jaxis.value > JSSENS )
						++evnt;
					else if( event.jaxis.value < -JSSENS )
						evnt +=2;
					else if( event.jaxis.value == 0){
						++evnt;
						down = 0;
					}
				}
				if( event.jaxis.axis == 1 ){ //Up-Down movement
					//up, down, release
					evnt += 2;
					if( event.jaxis.value > JSSENS )
						++evnt;
					else if( event.jaxis.value < -JSSENS )
						evnt +=2;
					else if( event.jaxis.value == 0){
						++evnt;
						down = 0;
					}
				}
				break;

			case SDL_JOYBUTTONUP:
				down = 0;
			case SDL_JOYBUTTONDOWN:
				evnt = SDLK_LAST + 7 + event.jbutton.button;
				break;
#endif

			//case SDL_MOUSEMOTION:
				//Camera::Rotate(event.motion.xrel % 5, 1.0, 0, 0.0);
				//Camera::Rotate(event.motion.yrel % 5, 0.0, 0, 1.0);
				//break;
				/*
				case SDL_MOUSEMOTION:
					MouseMoved(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;

				case SDL_MOUSEBUTTONUP:
					MouseButtonUp(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;

				case SDL_MOUSEBUTTONDOWN:
					MouseButtonDown(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;
				*/

		} // switch

		if( evnt > 0 )
			Bindings::process( evnt, down, unicode );

	} // while (handling input)

}
