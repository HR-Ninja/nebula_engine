#include "gl_backend.h"

void gl_init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void glad_init() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		exit(-1);
	}
}

void window_init(Window* window, const char* title, uint32_t width, uint32_t height) {
	assert(window);

	window->m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	check_window(window);

	glfwMakeContextCurrent(window->m_window);
}


void start_frame() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();
}

void end_frame(Window* window) {
	glfwSwapBuffers(window->m_window);
}

int window_active(const Window* window) {
	return !glfwWindowShouldClose(window->m_window);
}

void check_window(const Window* window) {
	if (window->m_window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		exit(-1);
	}
}