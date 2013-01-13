#version 120
#extension GL_ARB_explicit_attrib_location : require


//struct VSInput
//{
//	vec3 Position;
//	vec2 TexCoord;
//	vec3 Normal;
//};

uniform mat4 in_MVP;

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_TexCoord;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_Normal;


void main()
{
	gl_Position = in_MVP * vec4(in_Position, 1.0);
	vert_TexCoord   = in_TexCoord;
	vert_Normal     = vec3(0.0, 0.0, 1.0); //(gWorld * vec4(VSin.Normal, 0.0)).xyz;
	vert_WorldPos   = (in_MVP * vec4(in_Position, 1.0)).xyz; //(gWorld * vec4(VSin.Position, 1.0)).xyz;
}
