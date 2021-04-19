#version 450

in vec4 FragPos;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main(){ 
	
	float dist = length(FragPos.xyz - u_lightPos);
	dist = dist/u_farPlane;

	gl_FragDepth = dist;

}