varying vec2 v_uv;

uniform int u_mode;
uniform float u_aspect;
uniform sampler2D u_texture;
uniform int u_show_texture;

void main()
{
    vec2 uv = v_uv;                //get uv coordinates
    vec2 p = uv - vec2(0.5);        // center at (0,0)
    p.x *= u_aspect; 
	vec3 color = vec3(0.0);

    // ==================================
    // TASK 2.2 PROCEDURAL PATTERNS
    // ==================================
    if (u_show_texture == 0) 
    {
        if (u_mode == 0)
        {
            color = vec3(uv.x, 0.0, 1.0 - uv.x);
        }
        else if (u_mode == 1)
        {
            float d = length(p); 
            color = vec3(d); //the farther, the whiter
        }
        else if (u_mode == 2)
        {
            float red   = step(0.4, abs(fract(uv.x * 8.0) - 0.5)); 
            float blue  = step(0.4, abs(fract(uv.y * 6.0) - 0.5));
            color = vec3(red, 0.0, blue);
        }
        else if (u_mode == 3){
            float cols = 10.0 * u_aspect;
            float cellX = floor(uv.x * cols) / cols;
            float cellY = floor(uv.y * 10.0) / 10.0;
            color = vec3(cellX, cellY, 0.0);
        }
        else if (u_mode == 4){
            float cols = 10.0 * u_aspect;
            float x = floor(uv.x * cols);
            float y = floor(uv.y * 10.0);
            float c = mod(x + y, 2.0);
            color = vec3(c);
        }
        else
        {
            float wave = 0.5 + 0.3 * sin(uv.x * 6.28);
            float below = step(uv.y, wave);
            float intensity = mix((1.0 - uv.y) * 2.0, uv.y * 2.0, below);
            intensity = clamp(intensity, 0.0, 1.0);
            color = vec3(0.0, intensity, 0.0);
        }
    }
    // ==================================
    // TASK 2.3 IMAGE FILTERS
    // ==================================
    else 
    {
        vec3 texColor = texture2D(u_texture, uv).rgb;

        if (u_mode == 0)
        {
            // a) Grayscale
            float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
            color = vec3(luminance);
        }
        else if (u_mode == 1)
        {
            // b)
            color = 1.0 - texColor;
        }
        else if (u_mode == 2)
        {
            // c)
            color = texColor; // placeholder
        }
        else if (u_mode == 3)
        {
             // d)
             color = texColor; // placeholder
        }
        else if (u_mode == 4)
        {
             // e)
             color = texColor; // placeholder
        }
        else
        {
             // f)
             color = texColor; // placeholder
        }
    }

    gl_FragColor = vec4(color, 1.0);
}
