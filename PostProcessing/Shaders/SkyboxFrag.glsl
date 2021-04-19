#version 450 core   

in vec3 vs_texcoord;

out vec4 colour;

uniform samplerCube skybox;

void main() {

    vec3 envColor = texture(skybox, vs_texcoord).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    colour = vec4(envColor, 1.0);
}