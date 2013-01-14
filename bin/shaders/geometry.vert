#version 120

uniform mat4 in_MVP;

attribute vec3 in_Position;
attribute vec2 in_TexCoord;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_ColorMap;
varying vec3 vert_Normal;

uniform sampler2D in_ColorMap;

#ifdef _YNORMALS
	uniform sampler2D in_NormalMap;
#endif


#ifdef _YFIXED
	uniform vec3 in_Offset;
#endif


void main()
{
	vert_TexCoord   = in_TexCoord;
	vert_ColorMap	= texture2D(in_ColorMap, vert_TexCoord).rgb;
	vert_WorldPos   = (in_MVP * vec4(in_Position, 1.0)).xyz; //(gWorld * vec4(VSin.Position, 1.0)).xyz;

#ifdef _YNORMALS
	vert_Normal = normalize((texture2D(in_NormalMap, vert_TexCoord).rgb - 0.5) * 2.0);
#else
	vert_Normal = vec3(0.0, 0.0, 1.0); //(gWorld * vec4(VSin.Normal, 0.0)).xyz;
#endif


#ifdef _YFIXED
	vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif
	gl_Position 	= in_MVP * position;
}
