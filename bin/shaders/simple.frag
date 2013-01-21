#version 130

uniform sampler2D in_ColorMap;

in vec3 vert_WorldPos;
in vec2 vert_TexCoord;
in vec4 vert_Color;

out vec4 out_FragColor;


void main()
{
	vec4 color = texture2D(in_ColorMap, vert_TexCoord.st);
	out_FragColor = color * vert_Color;
}
