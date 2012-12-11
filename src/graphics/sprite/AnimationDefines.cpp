/*
 * AnimationDefines.cpp
 *
 *  Created on: 16.07.2012
 *
 */

#include "graphics/sprite/AnimationDefines.h"
#include "graphics/sprite/Animation.h"

#define ANIM( name ) name = Animation::getAnimationId(#name);

namespace AnimDef {

	unsigned int leftup;
	unsigned int leftdown;
	unsigned int rightup;
	unsigned int rightdown;
	unsigned int up;
	unsigned int down;
	unsigned int left;
	unsigned int right;

};

void AnimDef::init() {
	ANIM(leftup);
	ANIM(leftdown);
	ANIM(rightup);
	ANIM(rightdown);
	ANIM(up);
	ANIM(down);
	ANIM(left);
	ANIM(right);
}

#undef ANIM
