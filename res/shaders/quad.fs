varying vec2 v_uv;

uniform int u_mode;
uniform float u_aspect;

void main()
{
    vec2 uv = v_uv;                //get uv coordinates
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
        float cellX = floor(uv.x * cols) / cols; //says in which column we are, divide by cols to normalize
        float cellY = floor(uv.y * 10.0) / 10.0; //says in which row we are, divide by 10 to normalize
        color = vec3(cellX, cellY, 0.0); // cellX provides the red component and cellY the green one
    }

    else if (u_mode == 4){
        float cols = 10.0 * u_aspect; //same as before
        float x = floor(uv.x * cols);
        float y = floor(uv.y * 10.0);
        float c = mod(x + y, 2.0); //if x+y is even, it's white, if it's odd, it's black
        color = vec3(c);
    }

	else
    {
        // Sine wave divides the screen into two hemispheres
        // wave oscillates between 0.2 (right side) and 0.8 (left side)
        float wave = 0.5 + 0.3 * sin(uv.x * 6.28);

        // Determine which hemisphere the pixel is in:
        // below = 1.0 if uv.y <= wave (lower hemisphere)
        // below = 0.0 if uv.y > wave  (upper hemisphere)
        float below = step(uv.y, wave);

        // The gradient depends ONLY on the pixel's height (uv.y), not on the wave position.
        // Every pixel at the same y within the same hemisphere has the same color.
        // Lower hemisphere: uv.y * 2.0       -> y=0 is black(0), y=0.5 is full green(1)
        // Upper hemisphere: (1-uv.y) * 2.0   -> y=0.5 is full green(1), y=1 is black(0)
        // mix selects between the two based on which hemisphere we are in
        float intensity = mix((1.0 - uv.y) * 2.0, uv.y * 2.0, below);

        // Clamp to [0,1] since values can exceed 1.0 near the wave when it is far from center
        intensity = clamp(intensity, 0.0, 1.0);

        color = vec3(0.0, intensity, 0.0);
    }

    gl_FragColor = vec4(color, 1.0);
}
