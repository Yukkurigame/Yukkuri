#version 130


in vec3 in_Position;
in vec2 in_TexCoord;

uniform mat4 in_MVP;
uniform vec3 in_Offset;
uniform sampler2D in_ColorMap;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_ColorMap;



void main()
{
	vert_TexCoord   = in_TexCoord;
	vert_WorldPos   = (in_MVP * vec4(in_Position, 1.0)).xyz; //(gWorld * vec4(VSin.Position, 1.0)).xyz;
	vert_ColorMap	= texture2D(in_ColorMap, vert_TexCoord).rgb;

#ifdef _YFIXED
	vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif

	gl_Position = in_MVP * position;

}
