/*
 * Lighting.h
 *
 *  Created on: 08.01.2013
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

#include "graphics/GraphicsTypes.h"

enum LigthType {
	ltAmbient = 1, ltPoint = 2, ltSpot = 4
};


struct LightSource
{
	enum LigthType type;
	s4ub ambient;
	s4ub diffuse;
	s4ub specular;
	s3f position;
};



namespace LightingManager
{
	UINT addLight( enum LigthType type );
	LightSource* getLight( UINT id );
	void removeLight(  );
	void setupLights( );

}


#endif /* LIGHTING_H_ */
