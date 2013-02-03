/*
 * Mesh.cpp
 *
 *  Created on: 02.02.2013
 */

#include "graphics/sprite/Mesh.h"
#include "3rdparty/objTester/obj_parser.h"
#include "utils/list.h"
#include "utils/path.h"
#include "scripts/LuaConfig.h"

#include <string>
#include "config.h"
#include "debug.h"
#include "safestring.h"
#include "hacks.h"

extern MainConfig conf;


namespace  MeshManager {

	struct Mesh {
		UINT location;
		char* filename;
		obj_scene_data data;
	};

	list< Mesh* > Meshes;

	void load_brush( GLBrush* brush, obj_scene_data* data );
}


void MeshManager::init( )
{
	// Push shader config to stack and load config;
	LuaConfig* cfg = new LuaConfig();
	list< std::string > names;
	cfg->getSubconfigsList( "mesh", names );
	lua_State* L = cfg->getState();
	ITER_LIST( std::string, names ){
		Mesh* m = new Mesh();
		cfg->pushSubconfig( it->data.c_str(), "mesh" );
		LUA_GET_VALUE( "file", m->filename );
		LUA_GET_VALUE( "location", m->location );
		cfg->pop( 1 );

		char* path = Path::join( conf.path.meshes.c_str(), m->filename );
		parse_obj_scene( &m->data,  path );
		free( path );

		Meshes.push( m );
	}
	delete cfg;
}


void MeshManager::clean( )
{
	ITER_LIST( Mesh*, Meshes ){
		free(it->data->filename);
		delete it->data;
	}
}


void MeshManager::load( GLBrush* brush )
{
	obj_scene_data* data = NULL;
	ITER_LIST( Mesh*, Meshes ){
		if( it->data->location == brush->type )
			data = &it->data->data;
	}

	if( !data ){
		Debug::debug( Debug::GRAPHICS, "No mesh file for mesh type " + citoa(brush->type) + ".\n" );
		return;
	}

	load_brush( brush, data );
}


void MeshManager::load_brush( GLBrush* brush, obj_scene_data* data )
{
	brush->resize_verticles( data->vertex_count );
	VertexV2FT2FC4UI* arr = VBOArray::pointer( brush->point_index );
	for( int i=0; i < data->vertex_count; ++i ){
		obj_vector* p = data->vertex_list[i];
		arr[i].verticles = s3f(p->e[0], p->e[1], p->e[2]);
		// I wanted to load texture coordinates from .obj files too, but ... fuck you, blender.
	}
}
