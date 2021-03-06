

#include "graphics/Camera.h"
#include "graphics/utils/gl_uniforms.h"
#include "units/Unit.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define GLM_PTR (float*)glm::value_ptr


namespace Camera {

	struct CameraState {
		int TargetMode;
		double* TargetX;
		double* TargetY;
		Unit* Target;

		rect2f cam_view;
		glm::vec3 cam_position;
		glm::vec3 cam_offset;
		glm::mat4x4 projection;
		glm::mat4x4 view;
		glm::mat4x4 view_inversed;
		glm::mat4x4 model;

		list< glm::vec4 > rotations;

		glm::mat4x4 mvp;
		glm::mat4x4 movp;
		glm::mat4x4 mp;
		glm::mat4x4 mop;

		CameraState() :	TargetMode(ctmNormal), TargetX(NULL), TargetY(NULL), Target(NULL) {}
		CameraState( const CameraState* src ) :	TargetMode(src->TargetMode),
				TargetX(src->TargetX), TargetY(src->TargetY), Target(src->Target),
				cam_view(src->cam_view), cam_position(src->cam_position),
				cam_offset(src->cam_offset), projection(src->projection), view(src->view),
				model(src->model), mvp(src->mvp) {}
	};

	list< CameraState* > states;
	list< Brush* > faced_brushes;

	struct CameraShaders {
		int in_MVP;
		int in_MOVP;
	} camera_shaders;

	inline void update_viewport( )
	{
		if( !states.head )
			return;
		CameraState* state = states.head->data;

		// FIXME: Strange hack
		rect2f view( &state->cam_view );
		if( view.width < 0 ){
			view.width = state->cam_view.x;
			view.x = state->cam_view.x + state->cam_view.width;
		}
		if( view.height < 0 ){
			view.height = state->cam_view.y;
			view.y = state->cam_view.y + state->cam_view.height;
		}
		// FIXME: Why gl call here?
		glViewport( view.x, view.y, view.width, view.height );
	}

	void pass_state( const CameraState* state )
	{
		UniformsManager::pass_data( camera_shaders.in_MVP, GLM_PTR(state->mvp) );
		UniformsManager::pass_data( camera_shaders.in_MOVP, GLM_PTR(state->movp) );
	}

	glm::vec3 offset(  )
	{
		if( !states.head )
			return glm::vec3(0.0);
		CameraState* state = states.head->data;
		// I do not understand this properly, but this negation magic seems to work.
		glm::vec4 p = state->view * glm::vec4(-state->cam_offset.x, state->cam_offset.y, state->cam_offset.z, 1.0);
		//printf("%f:%f:%f\n", p.x, p.y, p.z);
		return glm::vec3( p.x, -p.y, -p.z );
	}
}


#define REG_UNIFORM( name, type )	\
		camera_shaders.name = UniformsManager::register_uniform( #name, type );

void Camera::init( )
{
	REG_UNIFORM( in_MVP, GL_FLOAT_MAT4 )
	REG_UNIFORM( in_MOVP, GL_FLOAT_MAT4 )
}

#undef REG_UNIFORM



void Camera::push_state( const rect2f* view, const s2f* z )
{
	CameraState* state = new CameraState;
	state->TargetMode = ctmNormal;
	state->TargetX = NULL;
	state->TargetY = NULL;
	state->Target = NULL;

	//glm::vec3 eye(1.0, 1.0, 1.0);
	//glm::vec3 target(0.0, 0.0, 0.0);
	//glm::vec3 up(0.0, 0.0, 1.0);
	//state->model_view = glm::lookAt( eye, target, up );

	state->cam_view = rect2f( view );
	state->projection = glm::ortho(view->x, view->x + view->width,
			view->y, view->y + view->height, z->x, z->y );
			//v( -wwidth*2.0, wwidth*2.0, -wheight*2.0, wheight*2.0 );
	state->view = glm::mat4x4(1.0);
	state->view_inversed = glm::mat4x4(1.0);
	state->model = glm::mat4x4(1.0);
	states.push( state );
	update_viewport( );
}

void Camera::push_state( const rect2f* view )
{
	s2f z(-10.0, 1.0);
	push_state(view, &z);
}

void Camera::push_state( )
{
	if( !states.head )
		return;
	states.push( new CameraState(states.head->data) );
}


void Camera::pop_state( )
{
	if( !states.head )
		return;
	delete states.head->data;
	states.remove( states.head, NULL );
	update( );
	update_viewport( );
}


int Camera::states_count( )
{
	return states.count;
}


float* Camera::inversed_rotation()
{
	if( !states.head )
		return NULL;
	return GLM_PTR(states.head->data->view_inversed);
}



void Camera::update( )
{
	if( !states.head )
		return;

	CameraState* state = states.head->data;
	if( state->TargetX && state->TargetY ){
		if( (*state->TargetX) != state->cam_position.x || (*state->TargetY) != state->cam_position.y ){
			Move(state->cam_position.x + (*state->TargetX),
					state->cam_position.y + (*state->TargetY), 0);
		}
	}


	glm::mat4 model = glm::translate( state->model, state->cam_position + offset() );
	state->mvp = state->projection * state->view * model;
	state->movp = state->projection * state->view; // * state->model

	pass_state( state );
}

s3f Camera::position( )
{
	s3f ret;
	if( states.head ){
		CameraState* state = states.head->data;
		glm::vec3 pos = -( state->cam_position + offset() );
		ret.x = pos.x;
		ret.y = pos.y;
		ret.z = pos.z;
	}
	return ret;
}


float Camera::getX( )
{
	return position().x;
}


float Camera::getY( )
{
	return position().y;
}


void Camera::Move( float x, float y, float z )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->cam_position -= glm::vec3(x, y, z);
}


void Camera::Translate( float x, float y, float z )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->model = glm::translate( state->model, glm::vec3( x, y, z ) );
}


void Camera::Rotate( float angle, float x, float y, float z )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->view = glm::rotate(state->view, angle, glm::normalize(glm::vec3(x, y, z)));
	glm::vec3 normals = -glm::normalize(glm::vec3(x, y, z));
	state->rotations.push( glm::vec4( normals.x, normals.y, normals.z, angle ) );
	state->view_inversed = glm::mat4x4(1.0);
	ITER_LIST( glm::vec4, state->rotations ){
		state->view_inversed = glm::rotate(state->view_inversed, it->data.w, glm::vec3(it->data));
	}
}



void Camera::ChangeMode( enum ctMode mode )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;

	if( mode == state->TargetMode )
		return;
	state->TargetMode = mode;
	switch( mode ){
		case ctmCenter:
			state->cam_offset.x = -state->cam_view.width / 2;
			state->cam_offset.y = -state->cam_view.height / 2;
			state->cam_offset.z = 0;
			break;
		case ctmNormal:
			state->cam_offset.x = state->cam_offset.y = 0;
			break;
	}
}


void Camera::SetTarget( Unit* u )
{
	DeleteTarget( );
	if( !states.head || !u )
		return;

	CameraState* state = states.head->data;
	state->Target = u;
	SetTarget( u->getUnitpX(), u->getUnitpY() );
}


void Camera::SetTarget( double* x, double* y )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->TargetX = x;
	state->TargetY = y;
	ChangeMode( ctmCenter );
	update( );
}


Unit* Camera::GetTarget( )
{
	if( !states.head )
		return NULL;
	CameraState* state = states.head->data;
	return state->Target;
}


void Camera::DeleteTarget( )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->Target = NULL;
	state->TargetX = NULL;
	state->TargetY = NULL;
	ChangeMode( ctmNormal );
}
