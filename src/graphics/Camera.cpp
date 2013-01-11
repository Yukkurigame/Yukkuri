

#include "graphics/Camera.h"
#include "units/Unit.h"
#include "config.h"
#include "graphics/utils/gl_shader.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



extern MainConfig conf;

#define GLM_PTR (float*)glm::value_ptr


namespace Camera {
	int TargetMode = ctmNormal;
	double* TargetX = NULL;
	double* TargetY = NULL;
	Unit* Target = NULL;

	glm::mat3 rotation;
	glm::vec3 cam_position;
	glm::vec3 cam_offset;
	glm::mat4x4 projection;
	glm::mat4x4 model;
	glm::mat4x4 model_view;
}



void Camera::init( )
{
	glm::vec3 eye(1.0, 1.0, 1.0);
	glm::vec3 target(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 0.0, 1.0);
	model_view = glm::lookAt( eye, target, up );
	double wwidth = conf.windowWidth;
	double wheight = conf.windowHeight;
	projection = glm::ortho(0.0, wwidth, 0.0, wheight, -10.0, 1.0);
	//			 glm::ortho( -wwidth*2.0, wwidth*2.0, -wheight*2.0, wheight*2.0, -30.0, 30.0 );
	model = glm::mat4x4(1.0);
}


void Camera::Update( )
{
	if( TargetX && TargetY ){
		if( (*TargetX) != cam_position.x || (*TargetY) != cam_position.y ){
			Translate(cam_position.x + (*TargetX),
					cam_position.y +(*TargetY), 0);
		}
	}

	glm::vec3 cam_translation = cam_position + cam_offset;
	glm::mat4 view = glm::translate(glm::mat4(1.0), cam_translation);

	glm::mat4x4 mvp = projection * view * model;
	Shaders::passUniformMatrix4fv( glsAll, "in_MVP", 1, GL_FALSE, GLM_PTR(mvp));
	Shaders::passUniform3fv( glsFixed, "in_Offset", 1, GLM_PTR(cam_translation));
}


float Camera::GetX( )
{
	if( TargetX != NULL ){
		if( TargetMode == ctmCenter ){
			return *TargetX - ( conf.windowWidth / 2 );
		}
		return *TargetX;
	}
	return cam_position.x;
}


float Camera::GetY( )
{
	if( TargetY != NULL ){
		if( TargetMode == ctmCenter ){
			return *TargetY - ( conf.windowHeight / 2 );
		}
		return *TargetY;
	}
	return cam_position.y;
}


void Camera::Translate( float x, float y, float z )
{
	cam_position -= glm::vec3(x, y, z);
}


void Camera::Rotate( float angle, float x, float y, float z )
{
	printf("rotation: %f\n", angle);
	model = glm::rotate(model, angle, glm::normalize(glm::vec3(x, y, z)));
}



void Camera::ChangeMode( enum ctMode mode )
{
	if( mode == TargetMode )
		return;
	TargetMode = mode;
	switch( mode ){
		case ctmCenter:
			cam_offset.x = (float)(conf.windowWidth >> 1);
			cam_offset.y = (float)(conf.windowHeight >> 1);
			break;
		case ctmNormal:
			cam_offset.x = cam_offset.y = 0;
			break;
	}
}


void Camera::SetTarget( Unit* u )
{
	DeleteTarget( );
	if( !u )
		return;
	Target = u;
	SetTarget( u->getUnitpX(), u->getUnitpY() );
}


void Camera::SetTarget( double* x, double* y )
{
	TargetX = x;
	TargetY = y;
	ChangeMode( ctmCenter );
	Update( );
}


Unit* Camera::GetTarget( )
{
	return Target;
}


void Camera::DeleteTarget( )
{
	Target = NULL;
	TargetX = NULL;
	TargetY = NULL;
	ChangeMode( ctmNormal );
}
