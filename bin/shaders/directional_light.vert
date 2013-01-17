#version 130

in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

uniform mat4 in_MVP;
uniform vec3 in_Offset;


void main() {

#ifdef _YFIXED
	vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif
	gl_Position = in_MVP * position;

}