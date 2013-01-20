#version 130

uniform sampler2D in_ColorMap;

#ifdef _YNORMALS
	uniform sampler2D in_NormalMap;
#endif

in vec2 vert_TexCoords;
in vec4 vert_Color;


#ifdef _YLIGHT
	varying vec3 vert_normal;
	struct lightSource {
		vec4 position;
		vec4 color;
	};
	uniform lightSource lights;

	vec3 scene_ambient = vec3(0.4, 0.4, 0.4);
#endif


out vec4 out_FragColor;


void main(void)
{
	vec4 color = texture2D(in_ColorMap, vert_TexCoords);

#ifdef _YLIGHT
	vec3 frag_normal = vec3(0.0, 0.0, 1.0);
	vec3 light_normal = normalize(lights.position.xyz);

	#ifdef _YNORMALS
		vec3 ncolor = normalize((texture2D(in_NormalMap, vert_TexCoords).rgb - 0.5) * 2.0);
		frag_normal = normalize(ncolor.rgb + frag_normal);
	#endif

	float d = dot(frag_normal, light_normal);

	float emissive_contribution = 0.01;
	float ambient_contribution  = 1.0;
	float diffuse_contribution  = 0.8;
	float specular_contribution = 0.9;

	//bool facing = d > 0.0;
	//vec4 frag_color = emissive_color * emissive_contribution +
	//        ambient_color * ambient_contribution  * c +
	//        diffuse_color  * diffuse_contribution  * c * max(d, 0) +
	//                (facing ?
	//        specular_color * specular_contribution * c * pow(max(0,dot(normal1, halfway_vector1)), 200.0) :
	//        vec4(0.0, 0.0, 0.0, 0.0));

	vec3 frag_color = scene_ambient * ambient_contribution +				// Ambient
					lights.color.rgb * max(0.0, d) * diffuse_contribution;	// Diffuse
	vec4 result_color = vec4(frag_color * color.rgb, color.a);
#else
	vec4 result_color = color;
#endif
	out_FragColor = vert_Color * result_color;
}
