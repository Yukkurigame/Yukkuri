/*
 * game.cpp
 *
 *  Created on: 27.11.2012
 */

#include "core/yukkuri.h"
#include "Define.h"
#include "config.h"
#include "3rdparty/timer/TimerManager.h"
#include "units/physics/physics.h"
#include "SDL/SDL.h"


// Global variables for extern use
long sdl_time = 0;
long last_timerevent_tick = 0;


// Private variables
namespace {
	int FPS_tick_counter;
	int FPS_counter;
	long last_tick;
}

// Gcc bug
extern MainConfig conf;
extern float currentFPS;


/** The main loop. **/
void Yukkuri::Start()
{
	Window.state = gsRunning;

	last_tick = 0;

	float UpdIterations = 0.0f;
	float CyclesLeft = 0.0f;

	long elapsed_ticks;
	float MaxCycles;
	float update_interval;
	int input_interval = 100;
	int input_ticks = 0;

	update_interval = 1.0f / conf.maxFrameRate;
	MaxCycles = conf.maxFrameRate / conf.minFrameRate;

	// Main loop.
	while( Window.state != gsEnd ){
		// Get current tick;
		sdl_time = SDL_GetTicks();

		// Calculate FPS.
		if( sdl_time - FPS_tick_counter >= 250 ){
			currentFPS = static_cast<float>( FPS_counter ) * 1000.0f /
								static_cast<float>( sdl_time - FPS_tick_counter );
			FPS_tick_counter = sdl_time;
			FPS_counter = 0;
		}

		// Process timer events
		if( sdl_time - last_timerevent_tick >= TIMEREVENTTICK ){
			Timer::ProcessTimerEvents();
			last_timerevent_tick = sdl_time;
		}

		elapsed_ticks = sdl_time - last_tick;

		input_ticks += elapsed_ticks;

		// Handle mouse and keyboard input
		if( input_ticks >= input_interval ){
			HandleInput();
			input_ticks = 0;
		}

		// Process physics only for running mode
		if( Window.state == gsRunning )
			cpSpaceStep( Phys::space, elapsed_ticks );


		if( Window.minimized ){
			// Release some system resources if the app. is minimized.
			//WaitMessage(); // pause the application until focus in regained
		}else{
			if( Window.state == gsRunning ){
				UpdIterations = elapsed_ticks + CyclesLeft;

				if( UpdIterations > ( MaxCycles * update_interval ) )
					UpdIterations = MaxCycles * update_interval;

				// Do some thinking
				while (UpdIterations > update_interval) { // Update game state a variable number of times
					UpdIterations -= update_interval;
					Think( elapsed_ticks );
				}
				CyclesLeft = UpdIterations;
			}
			last_tick = sdl_time;

			// Render frame
			Render();
			++FPS_counter;
			SDL_Delay(1);
		}
	}
}


void Yukkuri::End( )
{
	Window.state = gsEnd;
}


float Yukkuri::getFPS()
{
	return currentFPS;
}
