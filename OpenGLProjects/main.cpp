#pragma once

#include "Application.h"


bool InitializeWindowContext();

int main() {

	if (!InitializeWindowContext())
	{
		return -1;
	}
	
	Application *app = new BasicLightingApp();

	if (!app->Init()) {
		std::cout << "Failed to Initialize Application" << std::endl;
		return -1;
	}

	app->Run();

	return 0;
}

bool InitializeWindowContext() {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's function
	g_window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (g_window == nullptr) {
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(g_window);

	// GLFW Options
	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return 0;
	}

	//glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	return 1;
}


