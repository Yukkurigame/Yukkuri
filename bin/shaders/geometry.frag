#version 130

uniform sampler2D in_ColorMap;
uniform sampler2D in_NormalMap;

in vec3 vert_WorldPos;
in vec2 vert_TexCoord;
in vec4 vert_Color;


out vec4 frag_WorldPos;
//out vec3 frag_TexCoord;
out vec4 frag_ColorMap;
out vec4 frag_Normal;



void main()
{
	vec4 color = texture2D(in_ColorMap, vert_TexCoord.st);
	color *= vert_Color;
	if (color.a < 0.1)
		discard;

	frag_WorldPos = vec4(vert_WorldPos, 1.0); // out_WorldPos
	//frag_TexCoord = vec3(vert_TexCoord, 0.0); // out_TexCoord
	//frag_ColorMap = vert_ColorMap; // out_Diffuse
	frag_ColorMap = color;


#ifdef _YNORMALS
	vec3 normal = normalize((texture2D(in_NormalMap, vert_TexCoord.st).rgb - 0.5) * 2.0);
#else
	vec3 normal = vec3(0.0, 0.0, 1.0); //(gWorld * vec4(VSin.Normal, 0.0)).xyz;
#endif

	frag_Normal = vec4(normal, 1.0); // out_Normal
}
