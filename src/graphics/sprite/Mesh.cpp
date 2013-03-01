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
		GLuint type;
		char* filename;
		char* id;
		bool flat;
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
		LUA_GET_VALUE( "id", m->id );
		LUA_GET_VALUE( "file", m->filename );
		LUA_GET_VALUE( "type", m->type );
		LUA_GET_VALUE( "flat", m->flat );
		cfg->pop( 1 );
		free( it->data );

		char* path = Path::join( conf.path.meshes.c_str(), m->filename );

		if( parse_obj_scene( &m->data,  path ) ){
			Debug::debug( Debug::GRAPHICS, "Mesh %s loaded.\n", m->filename );
			m->location = Meshes.count;
			Meshes.push( m );
		}else{
			free(m->filename);
			free(m->id);
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
		free(it->data->id);
		delete it->data;
	}
}


int MeshManager::get( const char* name )
{
	ITER_LIST( Mesh*, Meshes ){
		if( !strcmp(it->data->id, name) )
			return it->data->location;
	}

	Debug::debug( Debug::GRAPHICS, "No mesh loaded with id %s.\n", name );
	return -1;
}


short MeshManager::load( GLBrush* brush )
{
	if( !brush || brush->mesh < 0 )
		return 0;

	Mesh* m = NULL;
	ITER_LIST( Mesh*, Meshes ){
		if( (int)it->data->location == brush->mesh ){
			m = it->data;
			break;
		}
	}

	if( !m ){
		Debug::debug( Debug::GRAPHICS, "No mesh file for mesh type %d.\n", brush->mesh );
		return 0;
	}

	if( m->type )
		brush->method = m->type;

	if( m->flat )
		brush->setFaced();

	load_brush( brush, &m->data );

	return 1;
}


// FIXME: Looks like shit
#define SET_INDEX( tgt, src, loc, type )	\
	if( tgt > src )					\
		loc |= type;

inline void init_brush_indices( GLBrush* brush, s3f& vmin, s2f& tmin )
{
	if( brush->points_count != 4 )
		return;

	VertexV2FT2FC4UI* arr = brush->points();
	for( UINT i = 0; i < brush->points_count; ++i ){
		const s3f& vetx = arr[i].verticles;
		const s2f& coords = arr[i].coordinates;
		brush->vertex_indices[i] = 0;
		brush->texture_indices[i] = 0;

		SET_INDEX( vetx.x, vmin.x, brush->vertex_indices[i], qcRight )
		SET_INDEX( vetx.y, vmin.y, brush->vertex_indices[i], qcBottom )
		SET_INDEX( vetx.x, vmin.z, brush->vertex_indices[i], qcFront )

		SET_INDEX( coords.x, tmin.x, brush->texture_indices[i], qcRight )
		SET_INDEX( coords.y, tmin.y, brush->texture_indices[i], qcBottom )
	}
}

#undef SET_INDEX


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
	s3f vmin(0.0f, 0.0f, 0.0f);

#define CHECK_MIN( dst, tgt )	\
	if( dst > tgt )		\
		dst = tgt;

	brush->indices_count = 0;
	for( int i = 0; i < data->face_count; ++i ){
		obj_face* face = data->face_list[i];

		for( int j = 0; j < face->vertex_count; ++j, ++brush->indices_count ){
			int vindex = face->vertex_index[j];
			int tindex = face->texture_index[j];

			brush->indices_list[brush->indices_count] = vindex + brush->point_index;

			obj_vector* v = data->vertex_list[vindex];
			brush->vertex_points[vindex] = arr[vindex].verticles = s3f( v->e[0], v->e[1], v->e[2] );
			CHECK_MIN( vmin.x, v->e[0] )
			CHECK_MIN( vmin.y, v->e[1] )
			CHECK_MIN( vmin.z, v->e[2] )


			if( tindex >=0 ){
				obj_vector* t = data->vertex_texture_list[tindex];
				arr[tindex].coordinates = s2f( t->e[0], t->e[1] );
				CHECK_MIN( tmin.x, t->e[0] )
				CHECK_MIN( tmin.y, t->e[1] )
			}
		}
	}

#undef CHECK_MIN

	init_brush_indices( brush, vmin, tmin );

}

