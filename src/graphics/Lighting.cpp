/*
 * Lighting.cpp
 *
 *  Created on: 15.01.2013
 */

#include "graphics/Lighting.h"
#include "graphics/utils/gl_uniforms.h"

#include "safestring.h"


namespace LightingManager {
	list<LightSource*> point_lights;
	list<LightSource*> direction_lights;
	int point_count = 0;
	int direction_count = 0;

	inline list<LightSource*>* array_by_type( enum LightType type )
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

	void pass_data( const char* prefix, const char* name, UINT type, void* data ){
		char* locname = (char*)malloc( (UINT)sizeof(char) * (strlen(prefix) + strlen(name) + 3 ) );
		sprintf( locname, "%s.%s", prefix, name );
		UINT location = UniformsManager::register_uniform( locname, type );
		free(locname);
		UniformsManager::pass_data( location, data );
	}


	void get_source_info( enum LightType type, char*& name, int*& count_ptr )
	{
		const char* prefix;
		const char* location = "in_%s_Light[%d]";

		switch( type ){
			case ltDirectional:
				count_ptr = &direction_count;
				prefix = "dir";
				break;
			default:
				count_ptr = &point_count;
				return;
				break;
		}
		name = new char[ strlen(location) + strlen(prefix) + 3 ];
		sprintf( name, location, prefix, *count_ptr );
	}

	void register_light( LightSource* source )
	{
		if( !source )
			return;

		int* count_ptr = NULL;
		char* name = NULL;
		get_source_info( source->type, name, count_ptr );

		// Add one light source
		(*count_ptr)++;

		pass_data( name, "direction", GL_FLOAT_VEC3, &source->direction );
		pass_data( name, "color", GL_FLOAT_VEC3, &source->color );
		pass_data( name, "ambient", GL_FLOAT, &source->ambient );
		pass_data( name, "diffuse", GL_FLOAT, &source->diffuse );

		delete[] name;
	}

	void unregister_light( LightSource* source )
	{
		if( !source )
			return;

		int* count_ptr = NULL;
		char* name = NULL;
		get_source_info( source->type, name, count_ptr );

		// Remove one light source
		(*count_ptr)++;

		pass_data( name, "direction", GL_FLOAT_VEC3, NULL );
		pass_data( name, "color", GL_FLOAT_VEC3, NULL );
		pass_data( name, "ambient", GL_FLOAT, NULL );
		pass_data( name, "diffuse", GL_FLOAT, NULL );

		delete[] name;
	}
}




void LightingManager::init()
{
	UINT pcount = UniformsManager::register_uniform( "point_lights", GL_INT );
	UINT dcount = UniformsManager::register_uniform( "dir_lights", GL_INT );
	UniformsManager::pass_data( pcount, &point_count );
	UniformsManager::pass_data( dcount, &direction_count );
}





LightSource* LightingManager::add_light( enum LightType type )
{
	list<LightSource*>* light_array = array_by_type( type );
	LightSource* new_light = new LightSource( type );
	light_array->push( new_light );
	register_light(new_light);
	return new_light;
}



void LightingManager::remove_light( LightSource* source )
{
	list<LightSource*>* light_array = array_by_type( source->type );
	unregister_light( source );
	light_array->remove( source );
	delete source;
}



listElement<LightSource*>* LightingManager::first( enum LightType type )
{
	list<LightSource*>* light_array = array_by_type( type );
	return light_array->head;
}

