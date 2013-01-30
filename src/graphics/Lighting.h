/*
 * Lighting.h
 *
 *  Created on: 08.01.2013
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

#include "graphics/GraphicsTypes.h"
#include "3rdparty/CUDataUser.h"
#include "basic_types.h"


class CUData;


enum LightType {
	ltAmbient = 1, ltDirectional, ltPoint, ltSpot
};


class LightSource : public CUDataUser
{
public:
	// Base
	enum LightType type;
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

protected:
	virtual CUData* createUData();
};



namespace LightingManager
{
	void init( );

	LightSource* add_light( enum LightType type );
	void remove_light( LightSource* );
	listElement<LightSource*>* first( enum LightType type );
}


#endif /* LIGHTING_H_ */
