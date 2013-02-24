#version 130

in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

out vec2 vert_TexCoords;
out vec4 vert_Color;

#ifdef _YFIXED
	uniform mat4 in_MOVP;
	mat4 MVP = in_MOVP;
#else
	uniform mat4 in_MVP;
	mat4 MVP = in_MVP;
#endif

void main(void)
{
	vert_TexCoords = in_TexCoord;
	vert_Color = in_Color;
	gl_Position = MVP * vec4(in_Position, 1.0);
}
