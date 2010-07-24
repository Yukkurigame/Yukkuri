#ifndef YCONFIGTYPES_H
#define YCONFIGTYPES_H

#include <string>
using std::string;

struct EntityDefs
{
	string Name;
	string imageName;
	int width;
	int height;
	int imagecols;
	int imagerows;
};

//TODO: ororoshenkiroro
struct AnimationDefs
{
	int down[2];
	int leftdown[2];
	int left[2];
	int leftup[2];
	int up[2];
	int rightup[2];
	int right[2];
	int rightdown[2];
	AnimationDefs(){
		down[0] = 0; down[1] = 0; leftdown[0] = 0; leftdown[1] = 0;
		left[0] = 0; left[1] = 0; leftup[0] = 0; leftup[1] = 0; up[0] = 0; up[1] = 0;
		rightup[0] = 0; rightup[1] = 0; right[0] = 0; right[1] = 0; rightdown[0] = 0; rightdown[1] = 0;

	}
};

#endif //YCONFIGTYPES_H
