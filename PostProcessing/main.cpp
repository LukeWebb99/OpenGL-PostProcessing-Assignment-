#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"	
#include "GuiLayer.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "PointLight.h"
#include "Skybox.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "RenderTarget.h"
#include "Material.h"

#pragma region Window/Camera/Render target

Window window("Window", true, 1460, 768);
Camera camera(glm::vec3(0.f, 2.f, 10.f), glm::vec3(0, 1, 0), 0, 0, 5, 110, 90.f); 
RenderTarget renderTarget(window.GetBufferWidth(), window.GetBufferHeight()); // render target class that manages framebuffer

#pragma endregion

#pragma region Shader

Shader shader; // main shader for drawing
Shader dirShadowShader; // shader for creating directional shadow maps
Shader omniShadowShader; // shader for creating omni shadow maps
Shader QuadShader; // shader that will be used for post processing effects

#pragma endregion

#pragma region Texture Setup

	Texture HDRI("Textures/HDRIs/herkulessaulen_4k.hdr"); //HDRI used for skybox
	Texture hdriCubemap; // later I will conver the hdri into a cubemap 
	Texture irradianceCubemap; //this texture will blur the hrdi cubemap for irradance
	Texture prefilterMap; //prefilers cube map for roughness in pbr
	Texture brdfLUTMap; //brdf look up texture
	Texture HeightMap("Textures/parallax_mapping_height_map.png"); //not used, needed for material class
	
	//create texture classes for objects to use
	Texture Gold_Albedo("Textures/PBR/Gold (Au)_schvfgwp_Metal/Albedo_4K__schvfgwp.jpg");
	Texture Gold_Normal("Textures/PBR/Gold (Au)_schvfgwp_Metal/Normal_4K__schvfgwp.jpg");
	Texture Gold_Roughness("Textures/PBR/Gold (Au)_schvfgwp_Metal/Roughness_4K__schvfgwp.jpg");
	Texture Gold_AO("Textures/PBR/Gold (Au)_schvfgwp_Metal/Metalness_4K__schvfgwp.jpg");
	Texture Gold_Metallic("Textures/PBR/Gold (Au)_schvfgwp_Metal/Metalness_4K__schvfgwp.jpg");
	
	Texture Rock_Albedo("Textures/PBR/Icelandic Cracked Rock_vclnajuew_Surface/Albedo_4K__vclnajuew.jpg");
	Texture Rock_Normal("Textures/PBR/Icelandic Cracked Rock_vclnajuew_Surface/Normal_4K__vclnajuew.jpg");
	Texture Rock_Roughness("Textures/PBR/Icelandic Cracked Rock_vclnajuew_Surface/Roughness_4K__vclnajuew.jpg");
	Texture Rock_AO("Textures/PBR/Icelandic Cracked Rock_vclnajuew_Surface/AO_4K__vclnajuew.jpg");
	Texture Rock_Metallic("Textures/PBR/Icelandic Cracked Rock_vclnajuew_Surface/Metallic_4K__vclnajuew.jpg");

	Texture Marble_Albedo	("Textures/PBR/Gold Inlay Marble Tiles_vdfjbfvv/Albedo_4K__vdfjbfvv.png");
	Texture Marble_Normal	("Textures/PBR/Gold Inlay Marble Tiles_vdfjbfvv/Normal_4K__vdfjbfvv.png");
	Texture Marble_Roughness("Textures/PBR/Gold Inlay Marble Tiles_vdfjbfvv/Roughness_4K__vdfjbfvv.png");
	Texture Marble_AO		("Textures/PBR/Gold Inlay Marble Tiles_vdfjbfvv/AO_4K__vdfjbfvv.png");
	Texture Marble_Metallic ("Textures/PBR/Gold Inlay Marble Tiles_vdfjbfvv/Metalness_4K__vdfjbfvv.png");

	//Scratched Polyvinylpyrrolidone Plastic_schcbgfp

#pragma endregion

#pragma region Material Setup

//create material, used to bind a set of texture before drawing
Material GoldMaterial(Gold_Albedo, Gold_Normal, Gold_Roughness, Gold_AO, Gold_Metallic, HeightMap);
Material RockMaterial(Rock_Albedo, Rock_Normal, Rock_Roughness, Rock_AO, Rock_Metallic, HeightMap);
Material MarbleMaterial(Marble_Albedo, Marble_Normal, Marble_Roughness, Marble_AO, Marble_Metallic, HeightMap);

#pragma endregion

#pragma region Light Setup

//create directional light
DirectionalLight dirLight(1024, 1024, 1, { 1, 1, 1 }, 0.f, { 0.5, -1, 0 });

//create point light
std::vector<PointLight> pointlights(1);

//create spot light
SpotLight spotLight({ 0, 5, 0 }, { 1, 1, 1, 1 }, { 0, -1, 0 }, 25, 20, 1024, 1024, 0.1, 100);

#pragma endregion

#pragma region Model/Mesh

Model monkeySmooth; //Load blender monkey with smooth normals
Model monkeyFlat; //Load blender monkey with non-smooth normals
Model quad; //used for quad for the floor

#pragma endregion

#pragma region Uniforms

// uniforms 
bool useSmoothMonkey = false;

bool useRotation = true;

bool useTexture = true;

bool spotlightFlickering = false;
float spotlightFlickeringSpeed = 1.f;

bool u_MonochromeToggle = false; 
bool u_WobbleToggle = false;	 
bool u_BlurToggle = false;		
float u_BlurStrength = 1.f;		

glm::vec3 albedo = {0.0, 0.5, 0.5};
float metallic = 0.5, ao = 1.f, roughness = 0.5;

float y = 0; // used for roation blender monkey
int textureNum = 1;

#pragma endregion

#pragma region Scene Pass
void RenderScene(Shader* shader) {

	shader->Bind(); // bind shader

	if (spotlightFlickering) { // if spotlight flickering was enabled 
		float v0 = glfwGetTime() * spotlightFlickeringSpeed;
		float v1 = (glm::fract(sin(v0)));
		spotLight.SetIntensity(v1 * 5000); // set intensity 
	}

	shader->SetVec3f(albedo, "u_albedo"); //set uniform for objects properties
	shader->Set1f(metallic, "u_metallic");
	shader->Set1f(roughness, "u_roughness");
	shader->Set1f(ao, "u_ao");

	dirLight.Bind(shader); // bind shadow map textures
	spotLight.Bind(shader, 0);
	pointlights[0].Bind(shader, 0);

	shader->Set1f(glfwGetTime(), "u_Time"); // set time 
	shader->SetVec3f(camera.GetCameraPosition(), "u_cameraPosition"); // set camera position
	shader->SetMat4f(camera.CalculateProjectionMatrix(window.GetBufferWidth(), window.GetBufferHeight()), "u_Projection", false); // set projection matrix
	shader->SetMat4f(camera.CalculateViewMatrix(), "u_View", false); // set view matrix
	
	switch (textureNum) // bind material based on GUI input
	{
	case 0:
		RockMaterial.Bind(); 
		break;

	case 1:
		GoldMaterial.Bind();
		break;

	case 2:
		MarbleMaterial.Bind();
		break;

	default:
		break;
	}
	

	shader->Set1i(useTexture, "u_usePRB"); // set uniform for using prb
	shader->SetMat4f(monkeySmooth.GetModel(), "u_Model", false); // set model marix

	if (useSmoothMonkey)
		monkeySmooth.Render(); // render smooth 
	else
		monkeyFlat.Render(); // render flat

	RockMaterial.Bind(); // bind rock material 

	shader->Set1i(1, "u_usePRB"); // set pbr
	shader->SetMat4f(quad.GetModel(), "u_Model", false); // set model

	quad.SetScale({ 5.f, 0.001, 5.f }); // scale 
	quad.SetTranslation({ 0.f, -2.5f, 0.f }); // translate 
	quad.SetRotation({ 180.f, 0.f, 0.f }); // rotate
	quad.Render(); // render

	if (useRotation) { 
		y += 10 * window.GetDeltaTime(); // add 10 * delta time to y
		monkeySmooth.SetRotation(glm::vec3(0, y, 0)); // set rotation to monkey model
	}
}
#pragma endregion

#pragma region Shadow Passes
void DirectionalShadowMapPass(DirectionalLight* dirLight) {

	glEnable(GL_CULL_FACE); // enable face culling for better shadow casting
	glCullFace(GL_FRONT); // cull front face
	dirShadowShader.Bind(); // bind directional shadow shader
	dirShadowShader.SetMat4f(dirLight->CalculateLightTransform(), "u_DirectionLightTransform", false); // set matrix

	glViewport(0, 0, dirLight->GetShadowMapPtr()->GetWidth(), dirLight->GetShadowMapPtr()->GetHeight()); // set view port to the resolution of the shadow
	dirLight->GetShadowMapPtr()->BindFBO(); // bind framebuffer
	glClear(GL_DEPTH_BUFFER_BIT); // clear

	RenderScene(&dirShadowShader); // render scene 
	renderTarget.Bind(window); // re-bind render target
	glDisable(GL_CULL_FACE); // disable face culling
};

void OmniShadowMapPass(PointLight* light) {

	glEnable(GL_CULL_FACE); // enable face culling
	glCullFace(GL_FRONT); // cull front face

	omniShadowShader.Bind(); // bind omni shadow shader

	omniShadowShader.SetVec3f(light->GetPosition(), "u_lightPos"); // set light position in shader
	omniShadowShader.Set1f(light->GetFarPlane(), "u_farPlane"); // set far plane 

	auto lightMatrices = light->CalculateLightTransform(); // get light transforms for projections

	omniShadowShader.SetMat4f(lightMatrices[0], "u_lightMatrices[0]", false); // set matrices in shader
	omniShadowShader.SetMat4f(lightMatrices[1], "u_lightMatrices[1]", false);
	omniShadowShader.SetMat4f(lightMatrices[2], "u_lightMatrices[2]", false);
	omniShadowShader.SetMat4f(lightMatrices[3], "u_lightMatrices[3]", false);
	omniShadowShader.SetMat4f(lightMatrices[4], "u_lightMatrices[4]", false);
	omniShadowShader.SetMat4f(lightMatrices[5], "u_lightMatrices[5]", false);

	glViewport(0, 0, light->GetShadowMapPtr()->GetWidth(), light->GetShadowMapPtr()->GetHeight()); // set viewport same size as shadow resolution

	light->GetShadowMapPtr()->BindFBO(); // bind framebuffer
	glClear(GL_DEPTH_BUFFER_BIT); // clear

	RenderScene(&omniShadowShader); // render

	renderTarget.Bind(window); // bind render target
	glDisable(GL_CULL_FACE); // disabled face culling

}
#pragma endregion

int main() {

	GuiLayer GuiLayer(window.GetWindow()); //Create GUI class
	Skybox skybox(&hdriCubemap); //create skybox using hdri texture

	pointlights[0] = PointLight({ 1, 1, 1, 0.f }, { 2.f, 5.0f, 0.0f }, 1024, 1024, 0.1, 100); // create point light in array

	//load all the needed shaders from files
	shader.CreateFromFile("Shaders/Vertex.glsl", "Shaders/Frag.glsl");
	dirShadowShader.CreateFromFile("Shaders/DirectionShadowMapVert.glsl", "Shaders/DirectionShadowMapFrag.glsl");
	omniShadowShader.CreateFromFile("Shaders/OmniShadowMapShaderVert.glsl", "Shaders/OmniShadowMapShaderGeom.glsl", "Shaders/OmniShadowMapShaderFrag.glsl");
	QuadShader.CreateFromFile("Shaders/QuadShaderVert.glsl", "Shaders/QuadShaderFrag.glsl");

	//create textures for skybox and pbr
	HDRI.CreateHDRI();
	hdriCubemap.CreateCubemapFromHDRI(HDRI);
	irradianceCubemap.CreateIrradianceTexture(&hdriCubemap);
	prefilterMap.CreatePrefilterMap(&hdriCubemap);
	brdfLUTMap.CreateBRDFLookUpTable();

	HeightMap.CreateTexture2D(); 

	Rock_Albedo.CreateTexture2D(); //create textures for rock material 
	Rock_Normal.CreateTexture2D();
	Rock_Roughness.CreateTexture2D();
	Rock_AO.CreateTexture2D();
	Rock_Metallic.CreateTexture2D();

	Gold_Albedo.CreateTexture2D(); //create textures for gold material 
	Gold_Normal.CreateTexture2D();
	Gold_Roughness.CreateTexture2D();
	Gold_AO.CreateTexture2D();
	Gold_Metallic.CreateTexture2D();

	Marble_Albedo.CreateTexture2D(); 
	Marble_Normal.CreateTexture2D();
	Marble_Roughness.CreateTexture2D();
	Marble_AO.CreateTexture2D();
	Marble_Metallic.CreateTexture2D();


	quad.Load("Models/quad.fbx"); // load quad
	quad.Create();

	monkeySmooth.Load("Models/monkey_smooth.obj"); // load blender monkey
	monkeySmooth.Create();
	monkeySmooth.SetScale(glm::vec3(2)); // scale monkey

	monkeyFlat.Load("Models/monkey_flat.obj"); // load blender monkey
	monkeyFlat.Create();

	shader.Bind();

	// bind texture to reserved texture units 
	irradianceCubemap.Bind(TU_IRRADIANCE);
	prefilterMap.Bind(TU_PREFILTER);
	brdfLUTMap.Bind(TU_BRDF);

	// set textures in shader
	shader.Set1i(TU_IRRADIANCE, "u_irradianceMap");
	shader.Set1i(TU_PREFILTER, "u_prefilterMap");
	shader.Set1i(TU_BRDF, "u_brdfLUT");

	// set textures in shader for pbr
	shader.Set1i(TU_ALBEDO, "u_AlbedoTexture");
	shader.Set1i(TU_NORMAL, "u_NormalTexture");
	shader.Set1i(TU_ROUGHNESS, "u_RoughnessTexture");
	shader.Set1i(TU_AO, "u_AOTexture");
	shader.Set1i(TU_METALLIC, "u_MetallicTexture");
	shader.Set1i(TU_DISPLACEMENT, "u_DisplacementTexture");

	shader.Unbind(); // unbind shader

	while (window.IsOpen()) {


		renderTarget.Bind(window); // bind render target 

		//Shadow Passes
		DirectionalShadowMapPass(&dirLight); // calculate directional shadow map
		OmniShadowMapPass(&pointlights[0]); // calculate point light shadow map
		OmniShadowMapPass(&spotLight); // calculate spot light shadow map

		//draw skybox
		skybox.Render(camera.CalculateViewMatrix(), camera.CalculateProjectionMatrix(window.GetBufferWidth(), window.GetBufferHeight()));

		//	draw scene
		RenderScene(&shader);

		renderTarget.Unbind(); // unbind render target

		window.Update();
		if (window.UpdateOnFocus()) {
			camera.mouseControl(window.GetXChange(), window.GetYChange(), window.GetDeltaTime());
			camera.keyControl(window.GetsKeys(), window.GetDeltaTime());
		}

		{ // GUI //
			GuiLayer.Begin(); // being gui 

			ImGui::Begin("GUI");
			ImGui::Text("Application average %.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			if (ImGui::CollapsingHeader("Render Options")) { // options for post-processing
				ImGui::Checkbox("Rotate Model", &useRotation);
				ImGui::Checkbox("Woggle", &u_WobbleToggle);
				ImGui::Checkbox("Monochrome", &u_MonochromeToggle);
				ImGui::Checkbox("Blur", &u_BlurToggle); ImGui::SameLine();
				ImGui::SliderFloat("Strength", &u_BlurStrength, 1, 5);
			}

			if (ImGui::CollapsingHeader("Texture Options")) { // options for textures
				ImGui::Checkbox("Use PBR Textures", &useTexture);
				if (!useTexture) {
					ImGui::ColorEdit3("Object Colour", (float*)&albedo);
					ImGui::DragFloat("Metallic", &metallic, 0.01, 0.f, 1.f);
					ImGui::DragFloat("AO", &ao, 0.01, 0.f, 1.f);
					ImGui::DragFloat("Roughness", &roughness, 0.01, 0.f, 1.f);
				}
				else {
					ImGui::RadioButton("Rock Material"  , &textureNum, 0); ImGui::SameLine();
					ImGui::RadioButton("Gold Material"  , &textureNum, 1); ImGui::SameLine();
					ImGui::RadioButton("Marble Material", &textureNum, 2);
				}
				ImGui::Checkbox("Use Smooth Normals", &useSmoothMonkey);
			}

			if (ImGui::CollapsingHeader("PointLight Options")) { // options for point lights
				ImGui::DragFloat3("PL Position", (float*)pointlights[0].GetPositionPtr(), 0.01, -25, 25);
				ImGui::ColorEdit3("PL Colour", (float*)pointlights[0].GetColourPtr());
				ImGui::SliderFloat("PL Intensity", pointlights[0].GetIntensityPtr(), 0.f, 5000.f);
			}

			if (ImGui::CollapsingHeader("DirectionLight Options")) {  // options for directional lights
				ImGui::ColorEdit3("DL Colour", (float*)dirLight.GetColourPtr());
				ImGui::SliderFloat3("DL Direction", (float*)(dirLight.GetDirectionPtr()), -1.f, 1.f);
				ImGui::DragInt("DL Shadow Filter Level", dirLight.GetFilterLevelPtr(), 1.f, 0.f, 10);
				ImGui::SliderFloat("DL Intensity", dirLight.GetIntensityPtr(), 0.f, 1.f);
			}

			if (ImGui::CollapsingHeader("SpotLight Options")) { // options for spot lights
				ImGui::ColorEdit3("SL Colour", (float*)spotLight.GetColourPtr());
				ImGui::DragFloat3("SL Position", (float*)spotLight.GetPositionPtr(), 0.01, -25, 25);
				ImGui::DragFloat3("SL Direction", (float*)spotLight.GetDirectionPtr(), 0.01, -1, 1);
				ImGui::DragFloat("SL InnerCutOff", spotLight.GetInnerCutOffPtr(), 0.01, 0, 1);
				ImGui::DragFloat("SL OutterCutOff", spotLight.GetOutterCutOffPtr(), 0.01, 0, 1);
				ImGui::Checkbox("Enable Flickering", &spotlightFlickering);
				if (spotlightFlickering) {
					ImGui::SliderFloat("SL Flickering Speed", &spotlightFlickeringSpeed, 0.f, 10);
				}
				else {
					ImGui::SliderFloat("SL Intensity", spotLight.GetIntensityPtr(), 0.f, 5000);
				}
			};

			ImGui::End();
		}

		QuadShader.Bind(); // bind shader for drawing the quad

		QuadShader.Set1i(0, "u_Frame"); // set framebuffer-texture 
		renderTarget.GetTexture()->Bind(0); // bind framebuffer-texture 

		QuadShader.Set1i(u_BlurToggle, "u_BlurToggle"); // set option for post-processing effects
		QuadShader.Set1f(u_BlurStrength, "u_BlurStrength");
		QuadShader.Set1i(u_MonochromeToggle, "u_MonochromeToggle");
		QuadShader.Set1i(u_WobbleToggle, "u_WobbleToggle");

		QuadShader.SetVec2f(glm::vec2(window.GetBufferWidth(), window.GetBufferHeight()), "u_Resolution"); // set window size 
		QuadShader.Set1f(glfwGetTime(), "u_Offset"); // set offset used for wobble effect

		renderQuad(); // draw quad

		GuiLayer.End();
		window.Clear(); // clear

	}


	return 0;

}