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

	void load_brush( GLBrush* brush, const obj_scene_data* data );
}


void MeshManager::init( )
{
	// Push shader config to stack and load config;
	LuaConfig* cfg = new LuaConfig();
	list< char* > names;
	cfg->getSubconfigsList( "mesh", names );
	lua_State* L = cfg->getState();
	ITER_LIST( char*, names ){
		Mesh* m = new Mesh();
		cfg->pushSubconfig( it->data, "mesh" );
		LUA_GET_VALUE( "file", m->filename );
		LUA_GET_VALUE( "location", m->location );
		cfg->pop( 1 );
		free( it->data );

		char* path = Path::join( conf.path.meshes.c_str(), m->filename );

		if( parse_obj_scene( &m->data,  path ) ){
			Debug::debug( Debug::GRAPHICS, "Mesh %s loaded.\n", m->filename );
			Meshes.push( m );
		}else{
			free(m->filename);
			delete m;
			Debug::debug( Debug::GRAPHICS, "Cannot load mesh %s.\n", m->filename );
		}

		free( path );
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
		Debug::debug( Debug::GRAPHICS, "No mesh file for mesh type %d.\n", brush->type );
		return;
	}

	load_brush( brush, data );
}

void MeshManager::load_brush( GLBrush* brush, const obj_scene_data* data )
{
	brush->resize_verticles( data->vertex_count );

	if( !data->face_count )
		return;

	VertexV2FT2FC4UI* arr = brush->points();

	// TODO: Probably lose 1 byte on each face.
	// It is a big loss for 2-faced plains and require more perd-perd actions.
	size_t list_size = sizeof(UINT) * data->face_count * MAX_VERTEX_COUNT;
	brush->indices_list = (UINT*)malloc( list_size );

	s2f tmin(0.0f, 0.0f);

	brush->indices_count = 0;
	for( int i = 0; i < data->face_count; ++i ){
		obj_face* face = data->face_list[i];

		for( int j = 0; j < face->vertex_count; ++j, ++brush->indices_count ){
			int vindex = face->vertex_index[j];
			int tindex = face->texture_index[j];

			brush->indices_list[brush->indices_count] = vindex + brush->point_index;

			obj_vector* v = data->vertex_list[vindex];
			obj_vector* t = data->vertex_texture_list[tindex];

			arr[vindex].verticles = s3f( v->e[0], v->e[1], v->e[2] );
			arr[tindex].coordinates = s2f( t->e[0], t->e[1] );

			if( tmin.x > t->e[0] )
				tmin.x = t->e[0];
			if( tmin.y > t->e[1] )
				tmin.y = t->e[1];
		}
	}

	if( brush->type != prQUADS )
		return;

	for( int i = 0; i < 4; ++i ){
		s2f& coords = arr[i].coordinates;
		brush->texture_indices[i] = 0;
		if( coords.x > tmin.x )
			brush->texture_indices[i] |= qcRight;
		if( coords.y > tmin.y )
			brush->texture_indices[i] |= qcBottom;
	}

}
