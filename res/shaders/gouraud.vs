// Variable global de CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;

// Variables para pasar al fragment shader
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec3 v_color; // Gouraud calcula el color por vértice

// TODO: Ejercicio 1.3 - Declarar uniformes para Gouraud (propiedades de luz, material y camara)
// uniform vec3 u_light_position;
// uniform vec3 u_light_color;
// uniform vec3 u_ambient_light;
// uniform float u_ka;
// uniform float u_kd;
// uniform float u_ks;
// uniform float u_shininess;
// uniform vec3 u_camera_position;

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// Convert local position to world space
	v_world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;

	// Convert local normal to world space
	v_normal = (u_model * vec4(gl_Normal.xyz, 0.0)).xyz;
    
    // TODO: Ejercicio 1.3 - Calcular iluminación de Gouraud en este vértice
    // vec3 N = normalize(v_normal);
    // vec3 L = normalize(u_light_position - v_world_position); // Vector de luz
    // vec3 V = normalize(u_camera_position - v_world_position); // Vector de vista
    
    // Calcula ambiente, difuso y especular...
    // vec3 ambient = ...
    // vec3 diffuse = ...
    // vec3 specular = ...
    // 
    // v_color = ambient + diffuse + specular;

	// Project the vertex using the view-projection matrix
	gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
