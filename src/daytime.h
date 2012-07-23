#ifndef TIMEOFDAY_H
#define TIMEOFDAY_H

namespace DayTime
{

	void init();
	void clean();
	void update( const unsigned int& dt );
	int getDay( );
	void loadInterface();

};

#endif //TIMEOFDAY_H
