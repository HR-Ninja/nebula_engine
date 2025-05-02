#ifndef GL_BACKEND
#define GL_BACKEND

#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include"stdio.h"
#include"assert.h"
#include <stdlib.h>

typedef struct window {
	GLFWwindow* m_window;
} Window;

void check_window(const Window* window);
void gl_init();
void glad_init();
void window_init(Window* window, const char* title, uint32_t width, uint32_t height);
void start_frame();
void end_frame(Window* window);
int window_active(const Window* window);
void check_window(const Window* window);

#endif // !GL_BACKEND
