/*
 * generator_constants.h
 *
 *  Created on: 03.03.2013
 */

#ifndef GENERATOR_CONSTANTS_H_
#define GENERATOR_CONSTANTS_H_

#include "map/Constants.h"


enum GeneratorColors
{
	// Basic
	gcBLACK = 0x000000, gcRED = 0xff0000, gcGREEN = 0x00ff00, gcBLUE = 0x0000ff,
	gcWHITE = 0xffffff, gcGRAY = 0xdddddd, gcWATERSHEDS = 0x6699ff, gcAQUA = 0x003333,
	gcDARKGREEN = 0x009900,

	// Features
	gcOCEAN = 0x44447a, gcCOAST = 0x33335a, gcLAKESHORE = 0x225588, gcLAKE = 0x336699,
	gcRIVER = 0x225588, gcMARSH = 0x2f6666, gcICE = 0x99ffff, gcBEACH = 0xa09077,
	gcROAD1 = 0x442211, gcROAD2 = 0x553322, gcROAD3 = 0x664433, gcBRIDGE = 0x686860,
	gcLAVA = 0xcc3333,

	// Gradients
	gcELEVATION_LOW = 0x008800, gcELEVATION_HIGH = 0xffff00,
	gcMOISTURE_LOW = 0xbbaa33, gcMOISTURE_HIGH = 0x4466ff,
	gcTEMPERATURE_LOW = 0x0088ff, gcTEMPERATURE_HIGH = 0xff8800,

	// Terrain
	gcSNOW = 0xffffff, gcTUNDRA = 0xbbbbaa, gcBARE = 0x888888, gcSCORCHED = 0x555555,
	gcTAIGA = 0x99aa77, gcSHRUBLAND = 0x889977, gcTEMPERATE_DESERT = 0xc9d29b,
	gcTEMPERATE_RAIN_FOREST = 0x448855, gcTEMPERATE_DECIDUOUS_FOREST = 0x679459,
	gcGRASSLAND = 0x88aa55, gcSUBTROPICAL_DESERT = 0xd2b98b,
	gcTROPICAL_RAIN_FOREST = 0x337755, gcTROPICAL_SEASONAL_FOREST = 0x559944
};

unsigned static const int BiomesColors[bLAST] = {
	gcWHITE, gcOCEAN, gcLAKE, gcBEACH, gcMARSH, gcICE, gcSNOW, gcTUNDRA, gcBARE, gcSCORCHED,
	gcTAIGA, gcSHRUBLAND, gcTEMPERATE_DESERT, gcTEMPERATE_RAIN_FOREST,
	gcTEMPERATE_DECIDUOUS_FOREST, gcGRASSLAND, gcTROPICAL_RAIN_FOREST,
	gcTROPICAL_SEASONAL_FOREST, gcSUBTROPICAL_DESERT
};

typedef enum GeneratorColors GeneratorColor;

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
