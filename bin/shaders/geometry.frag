#version 120

uniform sampler2D gColorMap;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_Normal;

//layout(location=0) out vec3 out_WorldPos;
//layout(location=1) out vec3 out_Diffuse;
//layout(location=2) out vec3 out_Normal;
//layout(location=3) out vec3 out_TexCoord;


void main()
{
	gl_FragData[0].rgb = vert_WorldPos; // out_WorldPos
	gl_FragData[1].rgb = texture2D(gColorMap, vert_TexCoord).xyz; // out_Diffuse
	gl_FragData[2].rgb = normalize(vert_Normal); // out_Normal
	gl_FragData[3].rgb = vec3(vert_TexCoord, 0.0); // out_TexCoord
}
