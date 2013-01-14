#version 120

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_ColorMap;
varying vec3 vert_Normal;


void main()
{
	gl_FragData[0].rgb = vert_WorldPos; // out_WorldPos
	gl_FragData[1].rgb = vert_ColorMap; // out_Diffuse
	gl_FragData[2].rgb = vert_Normal; // out_Normal
	gl_FragData[3].rgb = vec3(vert_TexCoord, 0.0); // out_TexCoord
}
