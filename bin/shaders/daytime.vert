#version 120

uniform vec3 offset;

void main( ){
	vec4 totalOffset = vec4(offset.x, offset.y, offset.z, 0.0);
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * ( gl_Vertex - totalOffset );
}