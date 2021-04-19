#version 450 core

layout (location = 0) in vec3 position; 

uniform mat4 u_Model;
uniform mat4 u_DirectionLightTransform; // projection * view

void main() {

    gl_Position = u_DirectionLightTransform * u_Model * vec4(position, 1.f);

}