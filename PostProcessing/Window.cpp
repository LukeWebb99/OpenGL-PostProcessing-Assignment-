#include "Window.h"

Window::Window(const char* title, GLboolean resizeable, GLint width, GLint height)
{
	m_title = title;
	m_resizeable = resizeable;
	m_width = width;
	m_height = height;
	m_window = nullptr;
	m_mouseFirstMove = false;
	m_updateOnFocues = true;
	Init();
}

Window::~Window()
{
	glfwTerminate();
}

void Window::Update() {

	glfwPollEvents(); // get events
	glfwGetFramebufferSize(m_window, &m_bufferWidth, &m_bufferHeight); //get framebuffer size
	SetViewport(); // set view port to window size
	
	cTime = static_cast<float>(glfwGetTime()); //calc Delta Time
	dt = cTime - lTime;
	lTime = cTime;

}

void Window::Clear() {
	
	glfwSwapBuffers(m_window); // swap framebuffer size
	
	glClearColor(1.f, 0.f, 0.f, 1.f); // clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear bits

}

bool Window::IsOpen()
{
	return !glfwWindowShouldClose(m_window); // while window is not closed 
}

bool Window::IsFocused() {

	return(glfwGetWindowAttrib(m_window, GLFW_FOCUSED)); // return if window is focused

}

bool Window::UpdateOnFocus()
{
	return m_updateOnFocues;
}

void Window::SetViewport()
{
	glViewport(0, 0, m_bufferWidth, m_bufferHeight); //set view port to buffer size
}

float Window::GetDeltaTime()
{
	return dt;
}

GLfloat Window::GetBufferWidth()
{
	return m_bufferWidth;
}

GLfloat Window::GetBufferHeight()
{
	return m_bufferHeight;
}

GLfloat Window::GetXChange()
{
	GLfloat theChange = m_xChange;
	m_xChange = 0.0f;
	return theChange;
}

GLfloat Window::GetYChange()
{
	GLfloat theChange = m_yChange;
	m_yChange = 0.0f;
	return theChange;
}

void Window::CreateCallbacks()
{
	glfwSetKeyCallback(m_window, HandleKeys); // attach functions to callbacks
	glfwSetCursorPosCallback(m_window, HandleMouse);
	glfwSetWindowSizeCallback(m_window, HandleResize);
}

void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode) {

	Window* l_window = static_cast<Window*>(glfwGetWindowUserPointer(window));// get window pointer

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // close window
	}

	if (key >= 0 && key < 1024) {

		if (action == GLFW_PRESS) // if key is pressed 
		{
			l_window->m_keys[key] = true;  // set key is pressed 
		}
		else if (action == GLFW_RELEASE) // if key is released 
		{
			l_window->m_keys[key] = false; // set key is released 
		}
	}

	if (l_window->m_keys[GLFW_KEY_LEFT_ALT]) { // if alt key pressed
		l_window->SetUpdateOnFocus(false); // set window to not focused
		glfwSetInputMode(l_window->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // change cursor to normal
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && l_window->m_keys[GLFW_KEY_LEFT_ALT]) {
		l_window->SetUpdateOnFocus(true); // set window to focused
		glfwSetInputMode(l_window->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // change cursor to disabled
	}

}

void Window::HandleMouse(GLFWwindow* window, double xPos, double yPos) {

	Window* t_window = static_cast<Window*>(glfwGetWindowUserPointer(window));// get window pointer

	if (t_window->m_mouseFirstMove) { //if mouse hasnt been moved since window opened 
		t_window->m_lastX = xPos; 
		t_window->m_lastY = yPos;
		t_window->m_mouseFirstMove = false; 
	}

	t_window->m_xChange = xPos - t_window->m_lastX; // get distance mouse has moved 
	t_window->m_yChange = t_window->m_lastY - yPos;
	
	t_window->m_lastX = xPos; // current becomes past 
	t_window->m_lastY = yPos;

}

void Window::HandleResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); // if window is resized set view port to match
}

void Window::Init() {

	//INIT GLFW
	if (!glfwInit()) {
		std::cout << "[GLFW]: ERROR\n";
		glfwTerminate();
	}
	else
		std::cout << "[GLFW]: OK\n";

	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, m_resizeable);

	
	// Create the window
	m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
	if (!m_window)
	{
		printf("[ERROR]: GLFW window didn't init..");
		glfwTerminate();
	}

	glfwGetFramebufferSize(m_window, &m_bufferWidth, &m_bufferHeight); //static frame buffer

	// Set the current context
	glfwMakeContextCurrent(m_window);
	glViewport(0, 0, m_bufferWidth, m_bufferHeight);

	CreateCallbacks();
	
	//INIT GLEW
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "[ERROR]: GLEW_INIT_FAILED \n";
		glfwTerminate();
	}
	else
		std::cout << "[GLEW]: OK\n" << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST); // enable depth test
	glfwSetWindowUserPointer(m_window, this); // set this window to current
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // set seamless cubemap option

}
