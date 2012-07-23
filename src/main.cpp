
#include <stdlib.h>
#include "yukkuri.h"

// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
	extern Yukkuri Engine;

	if( Engine.Init() ){
		Engine.SetTitle( "Yukkuri game" );
		Engine.Start();
		Engine.SetTitle( "Quitting..." );
		return 1;
	}

	return 0;
}

