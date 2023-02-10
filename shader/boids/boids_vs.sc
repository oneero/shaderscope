$input  a_position, i_data0
$output v_texCoord

#include "common.sh"
#include "uniforms.sh"

void main()
{
	// Since our verts form a simple quad filling the screen and the particles
	// do not have their own vertices in the world space, we need to apply 
	// MVP transformations to calculate the screen space texture coordinates.

	vec3  eye = mul(u_view, vec4(i_data0.xyz, 1.0) ).xyz;
	vec3  up = normalize(cross(eye, vec3(1.0, 0.0, 0.0) ) );
	vec3  right = normalize(cross(up, eye));
	float size = u_particleSize;
	vec3  position = eye + size * right * a_position.x + size * up * a_position.y;

	v_texCoord.xy = a_position;
	v_texCoord.z = i_data0.w;
	gl_Position = mul(u_proj, vec4(position, 1.0) );
}
