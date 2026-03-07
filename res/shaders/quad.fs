varying vec2 v_uv;

uniform int u_mode;
uniform float u_aspect;
uniform sampler2D u_texture;
uniform int u_show_texture;
uniform float u_time;

void main()
{
    vec2 uv = v_uv;                //get uv coordinates
    vec2 p = uv - vec2(0.5);        // center at (0,0)
    p.x *= u_aspect; 
	vec3 color = vec3(0.0);

    // TASK 2.2 PROCEDURAL PATTERNS
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
    // TASK 2.3 IMAGE FILTERS
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
            float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
            vec3 yellow = vec3(250.0, 237.0, 39.0) / 255.0;
            color = luminance * yellow;
        }
        else if (u_mode == 3)
        {
             // d)
             float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
             float threshold = 0.5;
             float binary = step(threshold, luminance);
             color = vec3(binary);
        }
        else if (u_mode == 4)
        {
             // e)
             float dist = length(uv - vec2(0.5)); //distance from the center
             float vignette = smoothstep(0.8, 0.2, dist);//1 in the center, 0 at the edges
             color = texColor * vignette;
        }
        else if (u_mode == 5)
        {
             // f)we need to implemet an averaging filter
             vec3 sum = vec3(0.0);
             float offset = 1.0 / 256.0;
             for (int i = -1; i <= 1; i++) {
                 for (int j = -1; j <= 1; j++) {
                    //get the sum of the the nine pixels around the current pixel
                     sum += texture2D(u_texture, uv + vec2(float(i), float(j)) * offset).rgb; 
                 }
             }
             color = sum / 9.0; //divide by 9 to get the average
        }
        else if (u_mode == 6)
        {
            // g) Animated Pixelization
            // Determine the amount of "blocks" or large pixels.
            // We use sin(u_time) so this amount oscillates between 20 and 120 over time.
            float pixels = 20.0 + 100.0 * (0.5 + 0.5 * sin(u_time));
            
            // Create a grid effect by rounding the UV coordinates.
            // By multiplying by 'pixels', we move to a larger coordinate space,
            // 'floor' removes the decimals (creating the block), and dividing back returns to [0,1].
            vec2 pixelated_uv = floor(uv * pixels) / pixels;
            
            color = texture2D(u_texture, pixelated_uv).rgb;
        }
        else if (u_mode == 7)
        {
            // h) Augmentic Radial Chromatic Aberration
            // Calculate the direction and distance from the center of the screen.
            vec2 dir = uv - 0.5;
            float dist = length(dir);
            
            // The effect's strength increases with distance from the center (dist)
            // and pulses rapidly using the u_time variable.
            float strength = 0.05 * dist * (1.1 + sin(u_time * 3.0));
            
            // Offset the red and blue channels in opposite directions based on the
            // direction from the center. The green channel remains fixed.
            color.r = texture2D(u_texture, uv - dir * strength).r;
            color.g = texture2D(u_texture, uv).g;
            color.b = texture2D(u_texture, uv + dir * strength).b;
        }
    }

    gl_FragColor = vec4(color, 1.0);
}
