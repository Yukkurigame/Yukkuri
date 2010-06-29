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
};

#endif //YCONFIGTYPES_H
