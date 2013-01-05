#version 120
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 Position;

varying vec2 texCoords;
varying vec3 vert_normal;
varying vec4 vert_color;
varying float normals_enabled;

uniform float normalsEnabled;


vec4 scene_ambient = vec4(0.4, 0.4, 0.4, 1.0);


void main(void)
{
	//texCoords = in_texCoords;
	texCoords = vec2(gl_MultiTexCoord0);
	mat4 mvp = gl_ModelViewProjectionMatrix;
	vert_color = gl_Color;
	vert_normal = vec3(0.0, 0.0, 1.0); //gl_Normal;
	normals_enabled = normalsEnabled;
	gl_Position = mvp * vec4(Position, 1.0); // gl_Vertex;
}
