#version 130

uniform sampler2D in_gNoLightMap;
uniform sampler2D in_gPositionMap;
uniform sampler2D in_gNormalMap;
uniform sampler2D in_gColorMap;
uniform vec2 in_ScreenSize;

uniform int dir_lights;
const int max_dir_lights = 3;

struct Light
{
	vec3 direction;
	vec3 color;
	// Intensity
	float ambient;
	float diffuse;
};

uniform Light in_dir_Light[max_dir_lights];

out vec4 out_Color;


vec4 general_ambient = vec4(0.2, 0.2, 0.2, 1.0);


vec4 calculate_light(Light light, vec3 position, vec3 normal)
{
	vec4 ambient = vec4(light.color, 1.0f) * light.ambient;
	float df = dot(normal, -light.direction);
	vec4 diffuse  = vec4(0, 0, 0, 0);
	vec4 specular = vec4(0, 0, 0, 0);

	if (df > 0) {
		diffuse = vec4(light.color, 1.0f) * light.diffuse * df;

		//vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
		//vec3 LightReflect = normalize(reflect(LightDirection, Normal));
		//float SpecularFactor = dot(VertexToEye, LightReflect);
		//SpecularFactor = pow(SpecularFactor, gSpecularPower);
		//if (SpecularFactor > 0) {
		//    SpecularColor = vec4(Light.Color, 1.0f) * gMatSpecularIntensity * SpecularFactor;
		//}
	}

	return ambient + diffuse + specular;
}

vec2 get_TexCoord() {
	return gl_FragCoord.xy / in_ScreenSize;
}


void main() {
	vec2 tex_coord = get_TexCoord();
	vec4 color = texture2D(in_gColorMap, tex_coord.st);
	vec4 lightless = texture2D(in_gNoLightMap, tex_coord.st);
	vec3 position = texture2D(in_gPositionMap, tex_coord.st).rgb;
	vec3 normal = normalize(texture2D(in_gNormalMap, tex_coord.st).rgb);

	vec4 dir_lighting = color * general_ambient;
	for( int i = 0; i < dir_lights; ++i ){
		dir_lighting += color * calculate_light(in_dir_Light[i], position, normal);
	}

	out_Color = mix(dir_lighting, lightless, lightless.a );
}
