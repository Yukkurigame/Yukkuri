#version 130

uniform sampler2D in_ColorMap;
uniform sampler2D in_NormalMap;

in vec2 vert_TexCoords;
in vec4 vert_Color;

out vec4 out_FragColor;



#ifdef _YLIGHT
	in vec3 vert_normal;

	const int max_dir_lights = 3;
	uniform int dir_lights;

	struct Light
	{
		vec3 direction;
		vec3 color;
		// Intensity
		float ambient;
		float diffuse;
	};

	uniform Light in_dir_Light[max_dir_lights];

	vec3 general_ambient = vec3(0.2, 0.2, 0.2);

	vec3 calculate_light(Light light, vec3 normal)
{
	//vec3 light_normal = normalize(lights.position.xyz);

	float d = dot(normal, -light.direction);

	//float emissive_contribution = 0.01;
	//float ambient_contribution  = 1.0;
	//float diffuse_contribution  = 0.8;
	//float specular_contribution = 0.9;
	//bool facing = d > 0.0;
	//vec4 frag_color = emissive_color * emissive_contribution +
	//        ambient_color * ambient_contribution  * c +
	//        diffuse_color  * diffuse_contribution  * c * max(d, 0) +
	//                (facing ?
	//        specular_color * specular_contribution * c * pow(max(0,dot(normal1, halfway_vector1)), 200.0) :
	//        vec4(0.0, 0.0, 0.0, 0.0));

	return light.color * light.ambient +				// Ambient
			light.color * max(0.0, d) * light.diffuse;	// Diffuse
}
#endif



void main(void)
{
	vec4 color = texture2D(in_ColorMap, vert_TexCoords);

#ifdef _YLIGHT
	vec3 frag_normal = vec3(0.0, 0.0, 1.0);
	#ifdef _YNORMALS
		vec3 ncolor = normalize((texture2D(in_NormalMap, vert_TexCoords).rgb - 0.5) * 2.0);
		frag_normal = normalize(ncolor.rgb + frag_normal);
	#endif

	//vec3 color_accum = vec3(0.0);
	//for(int i = 0; i < max_dir_lights; i++){
	vec3 color_accum = calculate_light(in_dir_Light[0], frag_normal);
	//color_accum += calculate_light(in_dir_Light[1], frag_normal);
	//color_accum += calculate_light(in_dir_Light[2], frag_normal);
	//}
	color_accum = clamp(color_accum, general_ambient, vec3(1.0));
	vec4 result_color = vec4(color_accum * color.rgb, color.a);
#else
	vec4 result_color = color;
#endif

	out_FragColor = vert_Color * result_color;
}
