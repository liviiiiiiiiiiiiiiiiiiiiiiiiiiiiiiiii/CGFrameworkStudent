uniform mat4 u_model;
uniform mat4 u_viewprojection;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// Convert local position to world space
	v_world_position = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;

	// Convert local normal to world space
	v_normal = (u_model * vec4( gl_Normal.xyz, 0.0)).xyz;

	// Project the vertex using the view-projection matrix
	gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
