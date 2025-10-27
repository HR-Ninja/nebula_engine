#ifndef ENGINE_H
#define ENGINE_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include "cglm/cglm.h"
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

#define MAX_KEYS (GLFW_KEY_LAST + 1)
#define MAX_BUTTONS (GLFW_MOUSE_BUTTON_LAST + 1)

enum {
    FAILURE = 0,
    SUCCESS = 1
};

typedef struct {
    double mouse_x, mouse_y;
    double mouse_delta_x, mouse_delta_y;

    bool keys[MAX_KEYS];
    bool prev_keys[MAX_KEYS];

    bool buttons[MAX_BUTTONS];
    bool prev_buttons[MAX_BUTTONS];

} InputState;

typedef struct {
    uint32_t screen_width;
    uint32_t screen_height;
    bool screen_has_resized;
} WindowState;

typedef uint32_t Shader;
typedef uint32_t Texture;

extern GLFWwindow* g_window;
extern InputState g_input_state;
extern WindowState g_window_state;

extern float delta_time;
const static float fixed_timestep = 0.02;

static inline bool window_active(GLFWwindow* window) {
	return !glfwWindowShouldClose(window);
}

static inline void begin_render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

static inline end_render(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

// -- ## CORE ## -- 
extern bool engine_init(GLFWwindow* window, const char* title, uint32_t width, uint32_t height);

extern void start_frame();
extern void end_frame();

// -- ## CALLBACKS ## -- 
extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
extern void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// -- ## INPUT ## -- 
extern inline void update_input_state();
extern inline bool is_key_down(int key);
extern inline bool is_key_pressed(int key);
extern inline bool is_key_released(int key);
extern inline bool is_mouse_button_pressed(int btn);

// -- ## UTILS ## -- 
extern void print_fps();

// -- ## SHADERS ## -- 
extern bool load_shader(Shader* s, const char* vert_src_path, const char* frag_src_path);
extern bool compile_shader(Shader* s, const char* vertexSource, const char* fragmentSource);
extern void use_shader(const Shader s);

// -- ## TEXTURES ## -- 
extern bool load_texture(Texture* t, const char* path);
extern void bind_texture(const Texture t);

#endif