#version 450 core                                 

layout (location = 0) in vec3 position; 
layout (location = 1) in vec2 textureCoords; 
layout (location = 2) in vec3 normal; 
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 vs_position;
out vec2 vs_texcoord;
out vec3 vs_normal;            

out vec4 vs_directionLightPosition;
out vec3 vs_cameraPosition;

uniform mat4 u_Model;                         
uniform mat4 u_Projection;  
uniform mat4 u_View;
uniform mat4 u_DirectionLightTransform; 
uniform vec3 u_cameraPosition;
                                     
void main()                                   
{       
    vs_position = vec4(u_Model * vec4(position, 1.f)).xyz;
    vs_texcoord = textureCoords;
    vs_normal = mat3(transpose(inverse(u_Model))) * normal;
    
    vs_directionLightPosition = u_DirectionLightTransform * u_Model * vec4(position, 1.f);
    vs_cameraPosition = u_cameraPosition;

    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.f);

}