#version 120

uniform vec4 colorLight;

void main( ){
	gl_FragColor = colorLight + gl_Color;
}