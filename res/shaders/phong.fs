varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;

// TODO: Ejercicio 1.4 - Uniformes para luces y material (igual que en Gouraud pero aquí en el fragment)
// uniform vec3 u_light_position;
// uniform vec3 u_light_color;
// uniform vec3 u_ambient_light;
// uniform float u_ka, u_kd, u_ks, u_shininess;
// uniform vec3 u_camera_position;

// TODO: Ejercicio 1.5 - Uniformes para texturas (color, specular, normal) 
// uniform sampler2D u_color_texture;
// uniform int u_use_color_texture;
// 
// uniform sampler2D u_specular_texture;
// uniform int u_use_specular_texture;
// 
// uniform sampler2D u_normal_texture;
// uniform int u_use_normal_texture;
// uniform mat4 u_model; // Nota: Para pasar las normales leídas de la textura de local a global

void main()
{
    // Normal base
    // vec3 N = normalize(v_normal);
    
    // TODO: Ejercicio 1.5 - Cargar y aplicar texura de normales (opcional mezclando con la base para suavizar)
    // if (u_use_normal_texture == 1) {
    //     vec3 normal_map = texture2D(u_normal_texture, v_uv).xyz;
    //     // Convertir de [0, 1] a [-1, 1]
    //     normal_map = (normal_map * 2.0) - 1.0;
    //     // Mapear normal a world space usando u_model y normalizar
    //     // ...
    //     // Modificar N
    // }
    
    // Factores base
    // vec3 kd_color = vec3(u_kd);
    // vec3 ks_color = vec3(u_ks);
    
    // TODO: Ejercicio 1.5 - Modificar el color base o factor especular si hay texturas activadas
    // if (u_use_color_texture == 1) {
    //     kd_color = texture2D(u_color_texture, v_uv).xyz;
    // }
    // if (u_use_specular_texture == 1) {
    //     // Alpha component of regular color texture or a specific specular texture
    //     ks_color = vec3(texture2D(u_specular_texture, v_uv).w); o .x
    // }

    // TODO: Ejercicio 1.4 - Calcular iluminacion de Phong aquí usando kd_color y ks_color
    // vec3 L = normalize(u_light_position - v_world_position);
    // vec3 V = normalize(u_camera_position - v_world_position);
    // ...
    // vec3 color = ambient + diffuse + specular;

    // Placeholder por ahora (para que compile)
    vec3 color = vec3(v_uv, 0.0);
    
	gl_FragColor = vec4(color, 1.0);
}
