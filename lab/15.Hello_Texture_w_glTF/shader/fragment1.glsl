#version 120// GLSL 1.20
varying vec3 v_normal_wc;
varying vec3 v_position_wc;
uniform vec4 u_material_ambient;
uniform vec4 u_material_specular;
uniform float u_material_shininess;
uniform vec3 u_view_position_wc;
uniform vec3 u_light_position_wc;
uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform sampler2D u_diffuse_texture0;
uniform sampler2D u_diffuse_texture1;
uniform sampler2D u_diffuse_texture2;
uniform sampler2D u_diffuse_texture3;
uniform sampler2D u_diffuse_texture4;
uniform sampler2D u_diffuse_texture5;
uniform sampler2D u_diffuse_texture6;
uniform sampler2D u_diffuse_texture7;
uniform sampler2D u_diffuse_texture8;
uniform int u_index;
varying vec2 v_texcoord;
uniform vec4 u_color;
vec4 calc_color(sampler2D u_diffuse_texture)
{
	vec4 color = vec4(0.0);
	vec3 n_wc = normalize(v_normal_wc);
	vec3 l_wc = normalize(u_light_position_wc - v_position_wc);
	vec3 r_wc = reflect(-l_wc, n_wc);
	vec3 v_wc = u_view_position_wc;
	color += (u_light_ambient * u_material_ambient);
	vec4 material_diffuse = texture2D(u_diffuse_texture, v_texcoord);
	float ndotl = max(0.0, dot(n_wc, l_wc));
	color += (ndotl * u_light_diffuse * material_diffuse);
	float rdotv = max(0.0, dot(r_wc, v_wc) );
	color += (pow(rdotv, u_material_shininess) * u_light_specular * u_material_specular);
	return color;
}
void main()
{
vec4 tmp_color;
if(u_index==0)
	tmp_color = texture2D(u_diffuse_texture0, v_texcoord);
else if(u_index==1)
	tmp_color = texture2D(u_diffuse_texture1, v_texcoord);
else if(u_index==2)
	tmp_color = texture2D(u_diffuse_texture2, v_texcoord);
else if(u_index==3)
	tmp_color = texture2D(u_diffuse_texture3, v_texcoord);
else if(u_index==4)
	tmp_color = texture2D(u_diffuse_texture4, v_texcoord);
else if(u_index==5)
	tmp_color = texture2D(u_diffuse_texture5, v_texcoord);
else if(u_index==6)
	tmp_color = texture2D(u_diffuse_texture6, v_texcoord);
else if(u_index==7)
	tmp_color = texture2D(u_diffuse_texture7, v_texcoord);
else
	tmp_color = texture2D(u_diffuse_texture8, v_texcoord);


	gl_FragColor = tmp_color;
}
