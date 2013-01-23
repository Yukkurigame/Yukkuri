#version 130

uniform sampler2D in_ColorMap;
uniform sampler2D in_NormalMap;

in vec3 vert_WorldPos;
in vec2 vert_TexCoord;
in vec4 vert_Color;


out vec4 frag_LightlessMap;
out vec4 frag_WorldPos;
out vec4 frag_Normal;
out vec4 frag_ColorMap;


void main()
{
	vec4 color = texture2D(in_ColorMap, vert_TexCoord.st);
	color *= vert_Color;
	if (color.a < 0.1)
		discard;

	frag_WorldPos = vec4(vert_WorldPos, 1.0); // out_WorldPos

#ifdef _YNORMALS
	vec3 normal = normalize((texture2D(in_NormalMap, vert_TexCoord.st).rgb - 0.5) * 2.0);
#else
	vec3 normal = vec3(0.0, 0.0, 1.0); //(gWorld * vec4(VSin.Normal, 0.0)).xyz;
#endif
	frag_Normal = vec4(normal, 1.0); // out_Normal

#ifdef _YLIGHT
	frag_ColorMap = color;
	frag_LightlessMap = vec4(0.0, 0.0, 0.0, 0.0);
#else
	frag_LightlessMap = color;
#endif
}
