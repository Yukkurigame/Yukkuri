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

	struct Ticks {
		long accumulator;
		long elapsed;
		long input;
		long timer;
		long last;

		Ticks( const long& time ) : accumulator(), elapsed(), input(),
				timer(), last(time) { }

		inline void update( const long& time ){
			elapsed = time - last;
			//if( elapsed > 250 )
			//	elapsed = 250

			accumulator += elapsed;

			// Add elapsed to input accumulator
			input += elapsed;

			// Set timer last tick to now if more than TIMEREVENTTICK passed
			if( time - timer >= TIMEREVENTTICK )
				timer = time;

			last = time;
		}
	};
}

// Gcc bug
extern MainConfig conf;
extern float currentFPS;


/** The main loop. **/
void Yukkuri::Start()
{
	Window.state = gsRunning;

	const float update_interval = 1.0f / conf.video.maxFrameRate * MILISECONDS;
	const float max_cycles = conf.video.maxFrameRate / conf.video.minFrameRate;
	float max_ticks = max_cycles * update_interval;

	sdl_time = SDL_GetTicks();

	Ticks ticks(sdl_time);

	// Main loop.
	while( Window.state != gsEnd ){
		// Get current tick;
		sdl_time = SDL_GetTicks();

		// Update various accumulators
		ticks.update( sdl_time );

		// Calculate FPS.
		if( sdl_time - FPS_tick_counter >= 250 ){
			currentFPS = static_cast<float>( FPS_counter ) * 1000.0f /
								static_cast<float>( sdl_time - FPS_tick_counter );
			FPS_tick_counter = sdl_time;
			FPS_counter = 0;
		}

		// Process timer events if timer set to now
		if( ticks.timer == sdl_time )
			Timer::ProcessTimerEvents();


		// Handle mouse and keyboard input
		if( ticks.input >= INPUT_INTERVAL ){
			HandleInput();
			ticks.input = 0;
		}

		if( Window.minimized ){
			// Release some system resources if the app. is minimized.
			//WaitMessage(); // pause the application until focus in regained
		}else{
			if( Window.state == gsRunning ){
				float current_ticks = ticks.accumulator;

				// Ajust max_ticks to make less calculations per frame.
				if( current_ticks < update_interval * 3 )
					max_ticks = max_cycles * update_interval;
				else
					max_ticks = update_interval;

				if( current_ticks > max_ticks )
					current_ticks = max_ticks;

				// Remove ticks that will be processed from accumulator
				ticks.accumulator -= current_ticks;

				// Update game state a variable number of times
				while( current_ticks >= update_interval ){
					cpSpaceStep( Phys::space, update_interval );
					Think( update_interval );
					current_ticks -= update_interval;
				}

				// Return any ticks left to accumulator
				ticks.accumulator += current_ticks;
			}

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
