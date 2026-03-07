// Variable global de CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;

// Variables para pasar al fragment shader
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec3 v_color; // Gouraud calcula el color por vértice

// TODO: Ejercicio 1.3 - Declarar uniformes para Gouraud (propiedades de luz, material y camara)
uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform vec3 u_ambient_light;
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;
uniform vec3 u_camera_position;

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// Convert local position to world space
    v_world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;
	// Convert local normal to world space
    v_normal = (u_model * vec4(gl_Normal.xyz, 0.0)).xyz;
    
    // TODO: Ejercicio 1.3 - Calcular iluminación de Gouraud en este vértice
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = normalize(2.0 * dot(L, N) * N - L);
    float dist = distance(u_light_position, v_world_position);
    float attenuation = 1.0 / (dist * dist);
    // Calcula ambiente, difuso y especular...

    // 1.3
    //ka*Ia
    vec3 ambient = u_ambient_light * u_ka; 

    //kd*Id*max(0,N.L)
    vec3 diffuse = u_light_color * u_kd * max(dot(L, N), 0.0);

    //ks*Is*max(0,R.V)^shininess
    float NdotL = max(dot(L, N), 0.0); // if the light does not hit the front face, there should not be a specular highlight either.

    vec3 specular = vec3(0.0);
    if (NdotL > 0.0)
        specular = u_light_color * u_ks * pow(max(dot(R, V), 0.0), u_shininess);

    //Ip = Ia*ka + Id*kd*max(0,N.L) + Is*ks*max(0,R.V)^shininess
    v_color = ambient + attenuation*(diffuse + specular);

	// Project the vertex using the view-projection matrix
	gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
