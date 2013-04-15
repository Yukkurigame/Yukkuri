/*
 * generator_constants.h
 *
 *  Created on: 03.03.2013
 */

#ifndef GENERATOR_CONSTANTS_H_
#define GENERATOR_CONSTANTS_H_

#include "map/Constants.h"

#define GENERATOR_NUM_POINTS 2000
// 0 to 1, fraction of water corners for water polygon
#define GENERATOR_LAKE_THRESHOLD 0.3
#define GENERATOR_NUM_LLOYD_ITERATIONS 4
#define GENERATOR_MIN_SIZE = 32;
#define GENERATOR_SIZE 600



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
	gmPolygons, gmWatersheds, gmBiome, gmSlopes, gmSmooth, gmElevation, gmMoisture,
	gmTemperature,
};

typedef enum GeneratorModes GeneratorMode;


enum NodeProperties {
	gnpElevation, gnpMoisture, gnpTemperature
};

typedef enum NodeProperties NodeProperty;

#endif /* GENERATOR_CONSTANTS_H_ */
