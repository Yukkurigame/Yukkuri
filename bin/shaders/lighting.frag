#version 120

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
varying vec2 texCoords; // the interpolated texture coordinates
varying vec3 vert_normal;
varying vec4 vert_color;
varying float normals_enabled;

struct lightSource {
	vec4 position;
	vec4 color;
};

uniform lightSource lights;



vec3 scene_ambient = vec3(0.4, 0.4, 0.4);



void main(void)
{
	vec4 color = texture2D(colorTexture, texCoords);
	vec3 ncolor = normalize((texture2D(normalTexture, texCoords).rgb - 0.5) * 2.0);

	vec3 frag_normal = normalize(vert_normal);
	vec3 light_normal = normalize(lights.position.xyz);

	if(normals_enabled != 0.0)
		frag_normal = normalize(ncolor.rgb + frag_normal);

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
	frag_color = frag_color * color.rgb;

	gl_FragColor = vert_color * vec4(frag_color, color.a);
}
