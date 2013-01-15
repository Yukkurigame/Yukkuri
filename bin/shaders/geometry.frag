#version 130

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_ColorMap;
varying vec3 vert_Normal;

out vec3 frag_WorldPos;
out vec3 frag_TexCoord;
out vec3 frag_ColorMap;
out vec3 frag_Normal;


void main()
{
	frag_WorldPos =  vert_WorldPos; // out_WorldPos
	frag_TexCoord = vert_ColorMap; // out_Diffuse
	frag_ColorMap = vert_Normal;   // out_Normal
	frag_Normal = vec3(vert_TexCoord, 0.0); // out_TexCoord
}
