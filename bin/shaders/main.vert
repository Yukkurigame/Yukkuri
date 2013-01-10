#version 120
//#extension GL_ARB_explicit_attrib_location : require

//layout(location = 0) in vec3 in_Position;

varying vec2 texCoords;
varying vec4 vert_color;

#ifdef _YLIGHT
	varying vec3 vert_normal;
#endif

#ifdef _YFIXED
	uniform vec3 offset;
#endif

void main(void)
{
	//texCoords = in_texCoords;
	vec3 in_Position = gl_Vertex.xyz;
	texCoords = vec2(gl_MultiTexCoord0);
	vert_color = gl_Color;

#ifdef _YLIGHT
	vert_normal = vec3(0.0, 0.0, 1.0); //gl_Normal;
#endif

#ifdef _YFIXED
	vec4 position = vec4(in_Position - offset, 1.0);
#else
	vec4 position = vec4(in_Position, 1.0);
#endif

	mat4 mvp = gl_ModelViewProjectionMatrix;
	gl_Position = mvp * position;
}