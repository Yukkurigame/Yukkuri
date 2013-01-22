

#include "graphics/Camera.h"
#include "units/Unit.h"
#include "graphics/utils/gl_shader.h"
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
		glm::mat3 rotation;
		glm::vec3 cam_position;
		glm::vec3 cam_offset;
		glm::mat4x4 projection;
		glm::mat4x4 model;
		glm::mat4x4 model_view;
	};

	list< CameraState* > states;

	inline void update_viewport( ){
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
}


void Camera::push_state( const rect2f* view )
{
	CameraState* state = new CameraState;
	state->TargetMode = ctmNormal;
	state->TargetX = NULL;
	state->TargetY = NULL;
	state->Target = NULL;

	glm::vec3 eye(1.0, 1.0, 1.0);
	glm::vec3 target(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 0.0, 1.0);
	state->model_view = glm::lookAt( eye, target, up );
	state->cam_view = rect2f( view );
	state->projection = glm::ortho(view->x, view->x + view->width,
			view->y, view->y + view->height, -10.0f, 1.0f);
			//v( -wwidth*2.0, wwidth*2.0, -wheight*2.0, wheight*2.0 );
	state->model = glm::mat4x4(1.0);
	states.push( state );
	update_viewport( );
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


void Camera::update( )
{
	if( !states.head )
		return;

	CameraState* state = states.head->data;
	if( state->TargetX && state->TargetY ){
		if( (*state->TargetX) != state->cam_position.x || (*state->TargetY) != state->cam_position.y ){
			Translate(state->cam_position.x + (*state->TargetX),
					state->cam_position.y +(*state->TargetY), 0);
		}
	}

	glm::vec3 cam_translation = state->cam_position + state->cam_offset;
	glm::mat4 view = glm::translate(glm::mat4(1.0), cam_translation);

	glm::mat4x4 mvp = state->projection * view * state->model;
	Shaders::passUniformMatrix4fv( glsAll, "in_MVP", 1, GL_FALSE, GLM_PTR(mvp));
	Shaders::passUniform3fv( glsFixed, "in_Offset", 1, GLM_PTR(cam_translation));
}


s3f Camera::position( )
{
	s3f ret;
	if( states.head ){
		CameraState* state = states.head->data;
		glm::vec3 pos = state->cam_position + state->cam_offset;
		ret.x = pos.x;
		ret.y = pos.y;
		ret.z = pos.z;
	}
	return ret;
}


float Camera::getX( )
{
	return position().x;
/*	if( !states.head )
		return 0;

	CameraState* state = states.head->data;

	if( state->TargetX != NULL ){
		if(  state->TargetMode == ctmCenter ){
			return * state->TargetX - ( state->cam_view.width / 2 );
		}
		return * state->TargetX;
	}
	return state->cam_position.x;
*/
}


float Camera::getY( )
{
	return position().y;
/*	if( !states.head )
		return 0;

	CameraState* state = states.head->data;

	if( state->TargetY != NULL ){
		if( state->TargetMode == ctmCenter ){
			return *state->TargetY - ( state->cam_view.height / 2 );
		}
		return *state->TargetY;
	}
	return state->cam_position.y;
*/
}


void Camera::Translate( float x, float y, float z )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->cam_position -= glm::vec3(x, y, z);
}


void Camera::Rotate( float angle, float x, float y, float z )
{
	if( !states.head )
		return;
	CameraState* state = states.head->data;
	state->model = glm::rotate(state->model, angle, glm::normalize(glm::vec3(x, y, z)));
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
			state->cam_offset.x = state->cam_view.width / 2;
			state->cam_offset.y = state->cam_view.height / 2;
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
