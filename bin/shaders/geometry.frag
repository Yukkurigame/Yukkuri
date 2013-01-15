#version 130

uniform sampler2D in_NormalMap;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_ColorMap;


//out vec3 frag_WorldPos;
out vec3 frag_TexCoord;
out vec3 frag_ColorMap;
out vec3 frag_Normal;


void main()
{
	//frag_WorldPos = vert_WorldPos; // out_WorldPos
	frag_TexCoord = vec3(vert_TexCoord, 0.0); // out_TexCoord
	frag_ColorMap = vert_ColorMap; // out_Diffuse

#ifdef _YNORMALS
	vec3 normal = normalize((texture2D(in_NormalMap, vert_TexCoord).rgb - 0.5) * 2.0);
#else
	vec3 normal = vec3(0.0, 0.0, 1.0); //(gWorld * vec4(VSin.Normal, 0.0)).xyz;
#endif

	frag_Normal = normal; // out_Normal
}
