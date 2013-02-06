#version 130

uniform sampler2D in_gNoLightMap;
uniform sampler2D in_gPositionMap;
uniform sampler2D in_gNormalMap;
uniform sampler2D in_gColorMap;
uniform vec2 in_ScreenSize;
uniform int in_CurrentLight;

uniform int point_lights;
const int max_point_lights = 3;

struct Light
{
	//vec3 direction;
	vec3 position;
	vec3 color;
	// Intensity
	float ambient;
	float diffuse;
};


uniform Light in_point_Light[max_point_lights];


out vec4 out_Color;


vec4 calculate_light(Light light, vec3 position, vec3 normal)
{
	vec3 direction = position - light.position;
	float distance = length(direction);
	direction = normalize(direction);

	vec4 ambient = vec4(light.color, 1.0) * light.ambient;
	float df = dot(normal, -direction);
	vec4 diffuse  = vec4(0, 0, 0, 0);
	vec4 specular = vec4(0, 0, 0, 0);

	if (df > 0) {
		diffuse = vec4(light.color, 1.0) * light.diffuse * df;

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

	vec4 pt_lighting = color * calculate_light(in_point_Light[in_CurrentLight], position, normal);

	//vec4 dir_lighting = vec4(0.0, 0.0, 0.0, 1.0);
	//for( int i = 0; i < dir_lights; i++ ){
		//dir_lighting += calculate_light(in_dir_Light[i], position, normal);
	//}

	//dir_lighting = color * clamp(dir_lighting, general_ambient, vec4(1.0));

	out_Color = mix(pt_lighting, lightless, lightless.a );
}

