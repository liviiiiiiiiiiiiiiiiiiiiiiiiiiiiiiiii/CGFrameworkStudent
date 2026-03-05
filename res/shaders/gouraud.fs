varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec3 v_color;

void main()
{
	// TODO: Ejercicio 1.3 - Gouraud Fragment
	// En Gouraud, el fragment shader simplemente usa el color interpolado calculado en el vertex shader
    
	gl_FragColor = vec4(v_color, 1.0);
}
