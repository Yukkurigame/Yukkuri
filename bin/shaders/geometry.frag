#version 120

uniform sampler2D in_ColorMap;

#ifdef _YNORMALS
	uniform sampler2D in_NormalMap;
#endif


varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_Normal;


void main()
{
	gl_FragData[0].rgb = vert_WorldPos; // out_WorldPos
	gl_FragData[1].rgb = texture2D(in_ColorMap, vert_TexCoord).xyz; // out_Diffuse

#ifdef _YNORMALS
	gl_FragData[2].rgb = normalize((texture2D(in_NormalMap, vert_TexCoord).rgb - 0.5) * 2.0);
#else
	gl_FragData[2].rgb = normalize(vert_Normal); // out_Normal
#endif

	gl_FragData[3].rgb = vec3(vert_TexCoord, 0.0); // out_TexCoord
}
