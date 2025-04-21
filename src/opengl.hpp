#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

class OpenGL {
public:

	// Singleton accessor
	static OpenGL& Instance() {
		static OpenGL instance;
		return instance;
	}

	// Window access
	GLFWwindow* GetWindow() {
		return m_window;
	}

	void SetWindow(const int& width, const int& height, const char* title) {
		
		if (m_window == NULL) {
			m_window = glfwCreateWindow(width, height, title, NULL, NULL);

			CheckWindow(m_window);

			glfwMakeContextCurrent(m_window);
			return;
		}

		glfwDestroyWindow(m_window);
		m_window = glfwCreateWindow(width, height, title, NULL, NULL);

		CheckWindow(m_window);

		glfwMakeContextCurrent(m_window);
	}

	bool WindowShouldClose() { return glfwWindowShouldClose(m_window); }

private:
	GLFWwindow* m_window = nullptr;

	OpenGL() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(800, 600, "Default", NULL, NULL);

		CheckWindow(m_window);

		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
	}

	~OpenGL() {

		if (m_window) {
			glfwDestroyWindow(m_window);
		}

		glfwTerminate();
	}

	void CheckWindow(const GLFWwindow* window) {
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
	}
};