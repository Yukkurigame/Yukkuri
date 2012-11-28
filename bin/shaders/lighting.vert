#version 120

uniform sampler2D colorTexture;
varying vec2 texCoords;

//attribute vec3 v_normal;
//uniform mat3 m_3x3_inv_transp;
varying vec4 color;

struct lightSource {
	vec4 position;
	vec4 diffuse;
};

uniform vec4 sun_position;
vec4 sun_color = vec4(1.0, 1.0, 1.0, 1.0); //uniform

//varying lightSource light_sun; // = lightSource(
//	vec4(1.0,  1.0,  1.0, 0.0),
//	vec4(1.0,  1.0,  1.0, 1.0)
//);

vec4 scene_ambient = vec4(0.4, 0.4, 0.4, 1.0);


void main(void)
{
	// FIXME: inverse in glsl 1.4
	texCoords = vec2(gl_MultiTexCoord0);
	mat4 mvp = gl_ModelViewProjectionMatrix;
	vec3 normalDirection = normalize(gl_Normal);
	vec3 lightDirection = normalize(vec3(sun_position));
	vec3 diffuseReflection =
		vec3(sun_color) // * vec3(mymaterial.diffuse)
		* max(0.0, dot(normalDirection, lightDirection));
	color = max(sun_position, scene_ambient);
	color = max(vec4(diffuseReflection, 1.0), scene_ambient);
	gl_Position = mvp * gl_Vertex;
}
