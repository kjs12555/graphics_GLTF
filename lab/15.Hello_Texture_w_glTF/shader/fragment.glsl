#version 120                  // GLSL 1.20

uniform sampler2D u_texture;

varying vec2 v_texcoord;
varying vec3 v_normal;

void main()
{
    gl_FragColor = texture2D(u_texture, v_texcoord);
}
