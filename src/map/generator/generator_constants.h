/*
 * generator_constants.h
 *
 *  Created on: 03.03.2013
 */

#ifndef GENERATOR_CONSTANTS_H_
#define GENERATOR_CONSTANTS_H_

#include "map/Constants.h"

enum ElevationGradientColors
{
	egcOCEAN = 0x008800, egcGRADIENT_LOW = 0x008800, egcGRADIENT_HIGH = 0xffff00
};

typedef enum ElevationGradientColors ElevationGradientColor;

enum MoistureGradientColors
{
	mgcOCEAN = 0x4466ff, mgcGRADIENT_LOW = 0xbbaa33, mgcGRADIENT_HIGH = 0x4466ff
};

typedef enum MoistureGradientColors MoistureGradientColor;

enum GeneratorModes
{
	gm3d, gmPolygons, gmWatersheds, gmBiome, gmSlopes, gmSmooth, gmElevation, gmMoisture,
	gmTemperature,

};

typedef enum GeneratorModes GeneratorMode;


enum NodeProperties {
	gnpElevation, gnpMoisture, gnpTemperature
};

typedef enum NodeProperties NodeProperty;

#endif /* GENERATOR_CONSTANTS_H_ */
