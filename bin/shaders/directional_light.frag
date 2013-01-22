#version 130

uniform sampler2D in_gPositionMap;
uniform sampler2D in_gColorMap;
uniform sampler2D in_gNormalMap;
uniform vec2 in_ScreenSize;

struct Light
{
	vec3 direction;
	vec3 color;
	// Intensity
	float ambient;
	float diffuse;
};

uniform Light in_Light;

out vec4 out_Color;


vec4 calculate_light(Light light, vec3 position, vec3 normal)
{
	vec4 ambient = vec4(0.4, 0.4, 0.4, 1.0);
	//if()
	//vec4 ambient = vec4(light.color, 1.0f) * light.ambient;
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
	vec3 position = texture2D(in_gPositionMap, tex_coord.st).rgb;
	vec4 color = texture2D(in_gColorMap, tex_coord.st);
	vec3 normal = normalize(texture2D(in_gNormalMap, tex_coord.st).rgb);

	out_Color = color * calculate_light(in_Light, position, normal);
}
