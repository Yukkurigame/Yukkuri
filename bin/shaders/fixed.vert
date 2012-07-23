#version 120

uniform vec3 offset;
varying vec2 texture_coordinate; 

void main( ){
	vec4 totalOffset = vec4(offset.x, offset.y, offset.z, 0.0);
	gl_FrontColor = gl_Color;
	texture_coordinate = vec2(gl_MultiTexCoord0); 
	gl_Position = gl_ModelViewProjectionMatrix * ( gl_Vertex - totalOffset );
}