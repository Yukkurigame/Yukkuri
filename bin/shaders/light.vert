#version 130

in vec3 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

uniform mat4 in_MOP;
uniform mat4 in_MOVP;
uniform mat4 in_MVP;
uniform mat4 in_MP;


void main() {

#ifdef _YFIXED
	#ifdef _YFACED
		mat4 MVP = in_MOP;
	#else
		mat4 MVP = in_MOVP;
	#endif
#else
	#ifdef _YFACED
		mat4 MVP = in_MP;
	#else
		mat4 MVP = in_MVP;
	#endif
#endif

	gl_Position = MVP * vec4(in_Position, 1.0);
}
