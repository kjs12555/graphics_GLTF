#version 120// GLSL 1.20
uniform vec4 u_color;
void main(){
	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	gl_FragColor = u_color;
}