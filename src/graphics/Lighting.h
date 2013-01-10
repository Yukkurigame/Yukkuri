/*
 * Lighting.h
 *
 *  Created on: 08.01.2013
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

#include "graphics/GraphicsTypes.h"

enum LigthType {
	ltAmbient = 1, ltDirectional = 2, ltPoint = 4, ltSpot = 8
};


struct LightSource
{
	// Base
	enum LigthType type;
	s4ub color;
	float ambient_intensity;
	float diffuse_intensity;
	// Directional && Spot
	s3f direction;
	// Point && Spot
	s3f position;
    struct {
        float Constant;
        float Linear;
        float Exp;
    } Attenuation;
    // Spot
    float cut_off;
};



namespace LightingManager
{
	UINT addLight( enum LigthType type );
	LightSource* getLight( UINT id );
	void removeLight(  );
	void setupLights( );

}


#endif /* LIGHTING_H_ */
