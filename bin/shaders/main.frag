#version 120

uniform sampler2D colorTexture;

#ifdef _YNORMALS
	uniform sampler2D normalTexture;
#endif

varying vec2 texCoords; // the interpolated texture coordinates
varying vec4 vert_color;

#ifdef _YLIGHT
	varying vec3 vert_normal;
	struct lightSource {
		vec4 position;
		vec4 color;
	};
	uniform lightSource lights;

	vec3 scene_ambient = vec3(0.4, 0.4, 0.4);
#endif


void main(void)
{
	vec4 color = texture2D(colorTexture, texCoords);

#ifdef _YLIGHT
	vec3 frag_normal = normalize(vert_normal);
	vec3 light_normal = normalize(lights.position.xyz);

	#ifdef _YNORMALS
		vec3 ncolor = normalize((texture2D(normalTexture, texCoords).rgb - 0.5) * 2.0);
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
	vec4 out_color = vec4(frag_color * color.rgb, color.a);
#else
	vec4 out_color = color;
#endif

	gl_FragColor = vert_color * out_color;
}