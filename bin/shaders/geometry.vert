#version 130

in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

out vec3 vert_WorldPos;
out vec2 vert_TexCoord;
out vec4 vert_Color;


#ifdef _YFIXED
	uniform mat4 in_MOVP;
	mat4 MVP = in_MOVP;
#else
	uniform mat4 in_MVP;
	mat4 MVP = in_MVP;
#endif



void main()
{

	vert_TexCoord   = in_TexCoord;
	vert_Color		= in_Color;

	vec4 position = MVP * vec4(in_Position, 1.0);

	vert_WorldPos = position.xyz;
	gl_Position = position;

}
