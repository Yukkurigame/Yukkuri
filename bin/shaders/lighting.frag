#version 120

uniform sampler2D colorTexture;
varying vec2 texCoords; // the interpolated texture coordinates
varying vec4 color;


void main(void)
{
	gl_FragColor = texture2D(colorTexture, texCoords) * color;
}
