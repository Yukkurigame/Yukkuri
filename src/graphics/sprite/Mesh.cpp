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
	VertexV2FT2FC4UI* arr = brush->points();

	/*for( int i=0; i < data->vertex_count; ++i ){
		obj_vector* p = data->vertex_list[i];
		arr[i].verticles = s3f(p->e[0], p->e[1], p->e[2]);

		// FIXME: by default all vertice i match tex_coord i; Need more complex alghoritm to use faces'
		if( !data->vertex_texture_count )
			continue;
		arr[i].coordinates = s2f(0, 0);
		// I wanted to load texture coordinates from .obj files too, but ... fuck you, blender.
	}
	*/

	if( !data->face_count )
		return;

	// TODO: Probably lose 1 byte on each face.
	// It is a big loss for 2-faced plains and require more perd-perd actions.
	brush->indices_list = (int*)malloc( (UINT)sizeof(int) * data->face_count * MAX_VERTEX_COUNT );

	brush->indices_count = 0;
	for( int i = 0; i < data->face_count; ++i ){
		obj_face* face = data->face_list[i];
		brush->indices_count += face->vertex_count;
		memcpy( &brush->indices_list[brush->indices_count],
				&face->vertex_index[0], face->vertex_count * (UINT)sizeof(int) );

		for( int j = 0; j < face->vertex_count; ++j ){
			int vindex = face->vertex_index[j];
			int tindex = face->texture_index[j];
			obj_vector* v = data->vertex_list[vindex];
			obj_vector* t = data->vertex_texture_list[tindex];

			arr[vindex].verticles = s3f( v->e[0], v->e[1], v->e[2] );
			arr[tindex].coordinates = s2f( t->e[0], t->e[1] );
		}
	}

/*


	int fic = 0;
	for( int i = 0; i < data->face_count; ++i ){
		obj_face* face = data->face_list[i];
		memcpy( &brush->indices_list[fic], &face->vertex_index[0], face->vertex_count * (UINT)sizeof(int) );
		fic += face->vertex_count;
	}
*/

}
