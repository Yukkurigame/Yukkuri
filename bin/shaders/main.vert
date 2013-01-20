#version 130

in vec3 in_Position;
in vec2 in_TexCoords;
in vec4 in_Color;

#ifdef _YFIXED
	uniform vec3 in_Offset;
#endif


out vec2 vert_TexCoords;
out vec4 vert_Color;

uniform mat4 in_MVP;


void main(void)
{
	vert_TexCoords = in_TexCoords;
	vert_Color = in_Color;

#ifdef _YFIXED
	vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif

	gl_Position = in_MVP * position;
}
