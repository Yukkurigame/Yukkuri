#version 120

uniform sampler2D colorTexture;
varying vec2 texture_coordinate;


void main( ){
	gl_FragColor = texture2D(colorTexture, texture_coordinate) * gl_Color;
}