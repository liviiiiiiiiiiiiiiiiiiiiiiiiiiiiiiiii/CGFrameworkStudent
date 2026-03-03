// This variables comes from the vertex shader
// They are baricentric interpolated by pixel according to the distance to every vertex
varying vec2 v_uv;

// Texture coming from the CPU
uniform sampler2D u_texture;

void main()
{
	// Sample the color texture at the interpolated UV coordinates
	gl_FragColor = texture2D(u_texture, v_uv);
}
