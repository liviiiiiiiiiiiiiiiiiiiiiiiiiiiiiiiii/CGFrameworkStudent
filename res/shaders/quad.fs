varying vec2 v_uv;

uniform int u_mode;
uniform float u_aspect;

void main()
{
    vec2 uv = v_uv;                
    vec2 p = uv - vec2(0.5);        // center at (0,0)
    p.x *= u_aspect; 
	vec3 color = vec3(0.0);

   if (u_mode == 0)
    {
        color = vec3(uv.x, 0.0, 1.0 - uv.x);
    }

	else if (u_mode == 1)
    {
        float d = length(p);
        color = vec3(d);
    }

	else if (u_mode == 2)
    {
        float red   = step(0.4, abs(fract(uv.x * 8.0) - 0.5));
        float blue  = step(0.4, abs(fract(uv.y * 6.0) - 0.5));
        color = vec3(red, 0.0, blue);
    }

    else if (u_mode == 3){ color = vec3(uv.x, uv.y, 0.0);}

    else if (u_mode == 4){
        float x = floor(uv.x * 10.0);
        float y = floor(uv.y * 10.0);
        float c = mod(x + y, 2.0);
        color = vec3(c);
    }

	else
    {
        float wave = 0.5 + 0.2 * sin(uv.x * 6.28);
        float band = step(abs(uv.y - wave), 0.05);
        color = mix(vec3(0.0, 0.2, 0.0), vec3(0.0, 0.8, 0.0), band);
    }

    gl_FragColor = vec4(color, 1.0);
}
