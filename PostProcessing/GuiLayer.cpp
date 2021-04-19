#include "GuiLayer.h"

#include "imgui.cpp"
#include "imgui_widgets.cpp"
#include "imgui_draw.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_glfw.cpp"

GuiLayer::GuiLayer()
{
	
}

GuiLayer::GuiLayer(GLFWwindow* window)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); //create contex
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark(); // set sytle colour
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings

	ImGui_ImplGlfw_InitForOpenGL(window, true); //into for opengl
	ImGui_ImplOpenGL3_Init("#version 450"); // pass opengl version
}

GuiLayer::~GuiLayer()
{
	ImGui_ImplOpenGL3_Shutdown(); //clean up
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GuiLayer::Begin()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GuiLayer::End()
{
	ImGui::Render(); //render gui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); //contex for opengl
}
