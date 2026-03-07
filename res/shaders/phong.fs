uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform vec3 u_ambient_light;

uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;
uniform vec3 u_camera_position;

uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;

uniform int u_use_color_texture;
uniform int u_use_normal_texture;
uniform int u_use_specular_texture;

uniform mat4 u_model;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_normal;

void main()
{
    vec3 N = normalize(v_normal);

    if (u_use_normal_texture == 1)
    {
        vec3 tex_normal = texture2D(u_normal_texture, v_uv).xyz;
        tex_normal = tex_normal * 2.0 - vec3(1.0);

        // normal map en espacio local del objeto
        N = normalize((u_model * vec4(tex_normal, 0.0)).xyz);
    }

    vec3 L = normalize(u_light_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = normalize(reflect(-L, N));

    float dist = distance(u_light_position, v_world_position);
    float attenuation = 1.0 / (dist * dist);

    float NdotL = max(dot(N, L), 0.0);

    vec3 Ka = u_ka;
    vec3 Kd = u_kd;
    vec3 Ks = u_ks;

    if (u_use_color_texture == 1)
    {
        vec4 tex = texture2D(u_color_texture, v_uv);
        Ka = tex.rgb;
        Kd = tex.rgb;
        Ks = vec3(tex.a);
    }

    if (u_use_specular_texture == 0)
        Ks = vec3(0.0);

    vec3 ambient = u_ambient_light * Ka;
    vec3 diffuse = u_light_color * Kd * NdotL;

    vec3 specular = vec3(0.0);
    if (NdotL > 0.0)
        specular = u_light_color * Ks * pow(max(dot(R, V), 0.0), u_shininess);

    vec3 color = ambient + attenuation * (diffuse + specular);
    gl_FragColor = vec4(color, 1.0);
}