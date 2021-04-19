#pragma once

#include <string>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GuiLayer
{
public:
	GuiLayer();
	GuiLayer(GLFWwindow* window);
	~GuiLayer();

	void Begin();
	void End();

private:
};
