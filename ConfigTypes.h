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
	int imgoffsetx;
	int imgoffsety;
	EntityDefs() {
		width = height = imagecols = imagerows = imgoffsetx = imgoffsety = 0;
	}
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
		down[0] = down[1] = leftdown[0] = leftdown[1] = left[0] = left[1] =  leftup[0] = leftup[1] = 0;
		up[0] = up[1] = rightup[0] = rightup[1] = right[0] = right[1] = rightdown[0] = rightdown[1] = 0;
	}
};

#endif //YCONFIGTYPES_H
