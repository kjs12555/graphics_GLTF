#version 120                  // GLSL 1.20

uniform mat4 u_PVM;           // Proj * View * Model

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;      // per-vertex color (per-vertex input)

varying   vec3 v_color;       // per-vertex color (per-vertex output)


uniform mat4 u_M;             // Model

uniform vec3 u_light_position;
uniform vec3 u_light_diffuse;
uniform vec3 u_material_diffuse;

vec3 calc_color()
{
  vec3 color = vec3(0);

  vec3 n_wc = (u_M * vec4(a_normal, 0)).xyz;
  vec3 p_wc = (u_M * vec4(a_position, 1)).xyz;

  vec3 l_wc = normalize(u_light_position - p_wc);
  float ndotl = max(0.0, dot(n_wc, l_wc));
  color += (ndotl * u_light_diffuse * u_material_diffuse);

  return color;
}


void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);
  //v_color = a_normal;
  v_color = calc_color();
}
