$input v_texCoord

#include "common.sh"
#include "uniforms.sh"

void main()
{
	vec3 color = mix(vec3(0.8f, 0.8f, 0.8f), 0.2 *  vec3(0.8, 0.8, 0.8), v_texCoord.z);
	float f = u_particleIntensity * pow(max(1.0 - length(v_texCoord.xy), 0.0), u_particlePower);
	gl_FragColor = vec4(color * f, 1.0);
}