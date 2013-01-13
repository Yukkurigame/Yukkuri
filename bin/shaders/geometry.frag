#version 120
#extension GL_ARB_explicit_attrib_location : require


uniform sampler2D gColorMap;

varying vec3 vert_WorldPos;
varying vec2 vert_TexCoord;
varying vec3 vert_Normal;

layout(location=0) out vec3 out_WorldPos;
layout(location=1) out vec3 out_Diffuse;
layout(location=2) out vec3 out_Normal;
layout(location=3) out vec3 out_TexCoord;


void main()
{
	out_WorldPos = vert_WorldPos;
	out_Diffuse  = texture2D(gColorMap, vert_TexCoord).xyz;
	out_Normal   = normalize(vert_Normal);
	out_TexCoord = vec3(vert_TexCoord, 0.0);
}
