//Global variables from the CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;

//Variables to pass to the fragment shader
varying vec2 v_uv;

void main()
{	
	//Pass texture coordinates to the fragment shader
	v_uv = gl_MultiTexCoord0.xy;

	//Convert local position to world space
	vec3 world_position = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;

	//Project the vertex using the model view projection matrix
	gl_Position = u_viewprojection * vec4(world_position, 1.0); //output of the vertex shader
}
