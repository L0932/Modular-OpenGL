#pragma once

#include <iostream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

#include <SOIL.h>

//GLM (OpenGL Mathematics. A header-only library)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"

//Window dimensions
extern const GLuint WIDTH;
extern const GLuint HEIGHT;

extern GLFWwindow* g_window;

// Camera
extern Camera camera;

extern GLfloat lastX;
extern GLfloat lastY;

extern bool keys[];

// Delta time
extern GLfloat deltaTime;
extern GLfloat lastFrame;

extern void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
extern void do_movement();

extern void SetCallbacks();

class Application
{
public:
	Application() {
		//m_pCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	}
	virtual ~Application(){}

	virtual bool Init() = 0;
	virtual void Run() = 0;

protected:
	virtual void CreateBuffers() = 0;
	virtual void CreateTextures() {}
	
	//Camera* m_pCamera;
};

class ModelLoadingApp : public Application
{
public:
	ModelLoadingApp() {}

	virtual bool Init() override;
	virtual void Run() override;
	virtual void CreateBuffers() override;
	virtual void CreateTextures() override;

	~ModelLoadingApp() {
		delete m_pMesh;
		delete m_pShader;
	}

private:
	OglDev::Mesh* m_pMesh;
	Shader* m_pShader;
	GLuint texture1;
	GLuint texture2;
	GLuint m_VBO, m_VAO;
};



class BasicLightingApp : public Application
{
public:
	BasicLightingApp() {
		//m_pCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		m_lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	}

	virtual bool Init() override;
	virtual void Run() override;
	virtual void CreateBuffers() override;
	virtual void CreateTextures() override;

	~BasicLightingApp() {}

private:
	glm::vec3 m_lightPos;
	Shader* m_pLightingShader;
	Shader* m_pLampShader;
	GLuint m_containerVAO, m_lightVAO;
};

class Basic3DSceneApp : public Application
{
public:
	Basic3DSceneApp() {}

	virtual bool Init() override;
	virtual void Run() override;
	virtual void CreateBuffers() override;
	virtual void CreateTextures() override;

	~Basic3DSceneApp() {
		//delete m_pMesh;
	}

private:

	Shader* m_pShader;
	GLuint texture1;
	GLuint texture2;
	GLuint m_VBO, m_VAO;
};
