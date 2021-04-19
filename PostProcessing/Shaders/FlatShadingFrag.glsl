#version 450

out vec4 colour;                               

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;

uniform samplerCube u_shadowMap;
uniform vec3 u_lightPosition;
uniform bool u_toggle;

void main() {
    
    vec3 lightDir = normalize(vs_position - u_lightPosition);
    float depth = texture(u_shadowMap, lightDir).r;
 
    colour = vec4(vs_position, 1.f);

   //if(u_toggle){
   //} else {
   //     colour = vec4(vec3(depth), 1.f);
   //}

}