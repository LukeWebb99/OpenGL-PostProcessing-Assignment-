#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoords;

out vec3 vs_position;
out vec2 vs_texcoord;

void main() {

	vs_position = position;
    vs_texcoord = textureCoords;

	gl_Position = vec4(position.x, position.y, 0.0, 1.0);

}