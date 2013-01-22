#version 130


in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

uniform mat4 in_MVP;
uniform vec3 in_Offset;

out vec3 vert_WorldPos;
out vec2 vert_TexCoord;
out vec4 vert_Color;

void main()
{
	vert_TexCoord   = in_TexCoord;
	vert_Color		= in_Color;

#ifdef _YFIXED
	vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif
	position = in_MVP * position;

	vert_WorldPos = position.xyz;
	gl_Position = position;

}
