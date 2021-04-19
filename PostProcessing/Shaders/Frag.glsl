#version 450 core                             

const float PI = 3.14159265359;

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 1;

struct PointLight {

    vec3 m_position;
    vec4 m_colour;

    float m_constant;
    float m_linear;
    float m_quadratic;

    //shadow
    samplerCube m_omniShadowMap;
    float m_omniFarPlane;

};

struct SpotLight {
    PointLight base;
    vec3 m_direction;
    float m_outterEdge;
    float m_innerEdge;
};

struct DirectionLight {
    vec4 m_colour;
    vec3 m_direction;
    int m_filterLevel;
    sampler2D m_shadowMap;
};

out vec4 colour;                               

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;            

in vec4 vs_directionLightPosition;
in vec3 vs_cameraPosition;

//lights
uniform PointLight pointLight[4];
uniform DirectionLight DirLight;
uniform SpotLight spotlight;

//pbr textures
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_AOTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_DisplacementTexture;

//skybox textures
uniform samplerCube u_irradianceMap;
uniform samplerCube u_prefilterMap;
uniform sampler2D   u_brdfLUT;

//PBR values for when no textures are used
uniform vec3 u_albedo;
uniform float u_metallic;  
uniform float u_roughness;
uniform float u_ao;



uniform bool u_usePRB;
uniform float u_Time;

vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float CalcOmniShadowFactor(PointLight light, int index) {

    vec3 lightDir = vs_position - light.m_position;
    float currDpeth = length(lightDir);

    float shadow = 0.f;
    float bias = 0.05;
    int samples = 10;

    float viewDirection = length(vs_cameraPosition - vs_position);
    float radius = (1.f + (viewDirection / light.m_omniFarPlane)) / 25.f;

    for(int i = 0; i < samples; i++) {
        float closestDepth = texture(light.m_omniShadowMap, lightDir + sampleOffsetDirections[i] * radius).r;
        closestDepth *= light.m_omniFarPlane;
        if(currDpeth - bias > closestDepth) {
            shadow += 1.f;
        }

    }

    shadow /= float(samples);
    return shadow;
}

float CalcOmniShadowFactor(SpotLight light, int index) {

    vec3 lightDir = vs_position - light.base.m_position;
    float currDpeth = length(lightDir);

    float shadow = 0.f;
    float bias = 0.05;
    int samples = 25;

    float viewDirection = length(vs_cameraPosition - vs_position);
    float radius = (1 + (viewDirection / light.base.m_omniFarPlane)) / 25.f;

    for(int i = 0; i < samples; i++) {
        float closestDepth = texture(light.base.m_omniShadowMap, lightDir + sampleOffsetDirections[i] * radius).r;
        closestDepth *= light.base.m_omniFarPlane;
        if(currDpeth - bias > closestDepth) {
            shadow += 1.0f;
        }

    }

    shadow /= float(samples);
    return shadow;
}


float CalcDirectionalShadowFactor(DirectionLight light, vec3 normal) {

	vec3 projCoords = vs_directionLightPosition.xyz / vs_directionLightPosition.w;
	projCoords = projCoords * 0.5 + 0.5;
	
    if(projCoords.z > 1.0) {
        return 0.f;
    }

	vec3 lightDir = normalize(-light.m_direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

	float shadow = 0.0;

    // PCF: percentage-closer filtering
    vec2 texelSize = 1.0 / textureSize(light.m_shadowMap, 0);
  
    for(int i = -light.m_filterLevel; i <= light.m_filterLevel; i++)
    {
        for(int j = -light.m_filterLevel; j <= light.m_filterLevel; j++)
        {
            float pcfDepth = texture(light.m_shadowMap, projCoords.xy + vec2(i, j) * texelSize).r; 
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    
    shadow /= ((light.m_filterLevel*2+1)*(light.m_filterLevel*2+1)); 

	return shadow;
}

vec3 CalcDirLight(DirectionLight light, vec3 albedo, vec3 normal, float metallic) {
 
    vec3 lightDir = normalize(light.m_direction);
    vec3 viewDir = (vs_cameraPosition - vs_position);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.f);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // combine results
    vec3 ambient  = light.m_colour.rgb * light.m_colour.w * F0;
    vec3 diffuse  = light.m_colour.rgb * light.m_colour.w * diff * F0;
    vec3 specular = light.m_colour.rgb * light.m_colour.w * spec  * metallic;

    return  ((1.0 - CalcDirectionalShadowFactor(DirLight, normal)) * (diffuse + specular));

} 


vec3 GetNormalFromMap() {

    vec3 tangentNormal = texture(u_NormalTexture, vs_texcoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vs_position);
    vec3 Q2  = dFdy(vs_position);
    vec2 st1 = dFdx(vs_texcoord);
    vec2 st2 = dFdy(vs_texcoord);

    vec3 N   = normalize(vs_normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {

    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
} 

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {

    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}   

vec3 CalcPointLight(PointLight light, vec3 view, vec3 albedo, vec3 normal, float roughness, float metallic, vec3 F0) {

        // calculate per-light radiance
        vec3 L = normalize(light.m_position - vs_position);
        vec3 H = normalize(view + L);
        float distance = length(light.m_position - vs_position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.m_colour.rgb * light.m_colour.w * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normal, H, roughness);   
        float G   = GeometrySmith(normal, view, L, roughness);    
        vec3 F    = fresnelSchlickRoughness(max(dot(H, view), 0.0), F0, roughness); //     
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(normal, L), 0.0); 

        return (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 CalcSpotLight(SpotLight light, vec3 albedo, vec3 normal, float metallic, float roughness, float ao) {
    
    vec3 V = normalize(vs_cameraPosition - vs_position);
    vec3 lightDir = normalize(-light.base.m_position - vs_position);
    float theta = dot(lightDir, normalize(light.m_direction));
    float epsilon = light.m_innerEdge - light.m_outterEdge;
    float hardness = clamp((theta -  light.m_innerEdge) / epsilon, 0.0, 1.0);

    float distance = length(light.base.m_position - vs_position);
    float attenuation = 1.0 / (distance * distance);
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 colour = CalcPointLight(light.base, V, albedo, normal, roughness, metallic, F0);

    float shadowFactor = CalcOmniShadowFactor(light, 0);

    if(theta > light.m_innerEdge) 
    {   
        return (1 - shadowFactor) * (colour * hardness) ;
    }

}
 

vec3 CalculatePBR(vec3 albedo, vec3 normal, float metallic, float roughness, float ao) {

    vec3 N = normal;
    vec3 V = normalize(vs_cameraPosition - vs_position);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)  {
        // add to outgoing radiance Lo
        Lo += (1 - CalcOmniShadowFactor(pointLight[i], i)) * CalcPointLight(pointLight[i], V, albedo, N, roughness, metallic, F0);
    }   
    
     // ambient lighting (we now use IBL as the ambient term)
    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	
    
    vec3 irradiance = texture(u_irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    return color;
} 

void main() {   

    vec3  l_albedo    = u_albedo;
    vec3  l_normal    = vs_normal;
    float l_metallic  = u_metallic;
    float l_roughness = u_roughness;
    float l_ao        = u_ao;

    if(u_usePRB) {
        l_albedo    = pow(texture(u_AlbedoTexture, vs_texcoord).rgb, vec3(2.2));
        l_normal    = GetNormalFromMap();  
        l_metallic  = texture(u_MetallicTexture, vs_texcoord).r;
        l_roughness = texture(u_RoughnessTexture, vs_texcoord).r;
        l_ao        = texture(u_AOTexture, vs_texcoord).r;
    }

     
    colour = vec4(CalculatePBR(l_albedo, l_normal, l_metallic, l_roughness, l_ao), 1.f)
    + vec4(CalcSpotLight(spotlight, l_albedo, l_normal, l_metallic, l_roughness, l_ao), 1.f)
    + vec4(CalcDirLight(DirLight, l_albedo, l_normal, l_metallic), 1.f);
    
       
}