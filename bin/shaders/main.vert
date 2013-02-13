#version 130

in vec3 in_Position;
in vec2 in_TexCoords;
in vec4 in_Color;

#ifdef _YFIXED
	uniform vec3 in_Offset;
#endif


out vec2 vert_TexCoords;
out vec4 vert_Color;


#ifdef _YFIXED
	uniform mat4 in_MOP;
	uniform mat4 in_MOVP;
#else
	uniform mat4 in_MVP;
	uniform mat4 in_MP;
#endif


//uniform mat4 in_M;
//uniform mat4 in_V;
//uniform mat4 in_P;


void main(void)
{
	vert_TexCoords = in_TexCoords;
	vert_Color = in_Color;

	//mat4 MVP = in_MVP;

#ifdef _YFIXED
	#ifdef _YFACED
		mat4 MVP = in_MOP;
	#else
		mat4 MVP = in_MOVP;
	#endif
	//mat4 V = in_V * (- vec4(in_Offset, 0.0));
	//vec4 position = vec4(in_Position - in_Offset, 1.0);
#else
	#ifdef _YFACED
		mat4 MVP = in_MP;
	#else
		mat4 MVP = in_MVP;
	#endif
	//mat4 V = in_V;
	//vec4 position = vec4(in_Position, 1.0);
#endif
	//vec4 position = vec4(in_Position, 1.0);

	//mat4 MVP = in_P * in_V;

	gl_Position = MVP * vec4(in_Position, 1.0);
}
