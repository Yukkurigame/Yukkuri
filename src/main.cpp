
#include "core/yukkuri.h"


// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
	if( Yukkuri::Init() ){
		Yukkuri::SetTitle( "Yukkuri game" );
		Yukkuri::Start();
		Yukkuri::SetTitle( "Quitting..." );
		Yukkuri::Clean();
		return 1;
	}

	return 0;
}

