/*
 * Lighting.cpp
 *
 *  Created on: 15.01.2013
 */

#include "graphics/Lighting.h"


namespace LightingManager {
	list<LightSource*> point_lights;
	list<LightSource*> direction_lights;


	inline list<LightSource*>* array_by_type( enum LigthType type )
	{
		switch(type){
			case ltAmbient:
			case ltDirectional:
				return &direction_lights;
				break;
			case ltPoint:
			case ltSpot:
				return &point_lights;
				break;
		}
		return NULL;
	}

}


LightSource* LightingManager::addLight( enum LigthType type )
{
	list<LightSource*>* light_array = array_by_type( type );
	LightSource* new_light = new LightSource();
	light_array->push( new_light );
	new_light->type = type;
	return new_light;
}


void LightingManager::freeLight( LightSource* source )
{
	list<LightSource*>* light_array = array_by_type( source->type );
	light_array->remove( source );
	delete source;
}


listElement<LightSource*>* LightingManager::first( enum LigthType type )
{
	list<LightSource*>* light_array = array_by_type( type );
	return light_array->head;
}

