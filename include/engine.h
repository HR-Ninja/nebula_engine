#pragma once
#include "hecs.h"
#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include "stb_image.h"
#include "cglm/cglm.h"
#include"stdio.h"
#include <stdlib.h>

// -----------------------------------------------------------------------------------------------

#define MAX_KEYS (GLFW_KEY_LAST + 1)
#define MAX_BUTTONS (GLFW_MOUSE_BUTTON_LAST + 1)

typedef uint32_t Shader;
typedef uint32_t Texture;

typedef struct {
    double mouse_x, mouse_y;
    double mouse_delta_x, mouse_delta_y;

    bool keys[MAX_KEYS];
    bool prev_keys[MAX_KEYS];

    bool buttons[MAX_BUTTONS];
    bool prev_buttons[MAX_BUTTONS];

} InputState;

static GLFWwindow* window;
static InputState input;
static float last_time;
static float delta_time;

// -----------------------------------------------------------------------------------------------

// Core
void engine_init(const char* title, uint32_t width, uint32_t height);
void start_frame();
void end_frame();
void check_window();
bool window_active();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void update_input_state();

bool is_key_down(int key);
bool is_key_pressed(int key);
bool is_key_released(int key);

bool is_mouse_button_pressed(int btn);

// Render
void print_fps();

// Shaders
void load_shader(Shader* s, const char* vert_src_path, const char* frag_src_path);
void compile_shader(Shader* s, const char* vertexSource, const char* fragmentSource);
void use_shader(const Shader s);
void check_location(uint32_t location);

// Textures
void load_texture(Texture* t, const char* path);
void bind_texture(const Texture t);


// -----------------------------------------------------------------------------------------------

void engine_init(const char* title, uint32_t width, uint32_t height) {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
	check_window();

	glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		exit(-1);
	}

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

void start_frame() {
    float current_time = glfwGetTime();
    delta_time = current_time - last_time;
    last_time = current_time;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
    update_input_state();
}

void end_frame() {
	glfwSwapBuffers(window);
    input.mouse_delta_x = 0;
    input.mouse_delta_y = 0;
}

bool window_active() {
	return !glfwWindowShouldClose(window);
}

void check_window() {
	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		exit(-1);
	}
}

void load_shader(Shader* s, const char* vert_src_path, const char* frag_src_path) {
    FILE* vert = fopen(vert_src_path, "rb");
    FILE* frag = fopen(frag_src_path, "rb");

    if(!vert || !frag) {
        perror("Failed to open file");
        exit(-1);
    }

    fseek(vert, 0, SEEK_END);
    long vert_length = ftell(vert);
    rewind(vert);   

    fseek(frag, 0, SEEK_END);
    long frag_length = ftell(frag);
    rewind(frag);   

    char* vert_buffer = (char*)malloc(vert_length + 1);
    char* frag_buffer = (char*)malloc(frag_length + 1);

    if (!vert_buffer || !frag_buffer) {
        perror("Failed to allocate memory");
        fclose(vert);
        fclose(frag);
        exit(-1);
    }

    fread(vert_buffer, 1, vert_length, vert);
    vert_buffer[vert_length] = '\0';
    fclose(vert);

    fread(frag_buffer, 1, frag_length, frag);
    frag_buffer[frag_length] = '\0';
    fclose(frag);

    compile_shader(s, vert_buffer, frag_buffer);
    free(vert_buffer);
    free(frag_buffer);
}

void compile_shader(Shader* s, const char* vertexSource, const char* fragmentSource) {
    // TODO: Added error checks
    uint32_t vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    *s = glCreateProgram();
    glAttachShader(*s, vertex);
    glAttachShader(*s, fragment);
    glLinkProgram(*s);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void use_shader(const Shader s) {
    static Shader current_shader = 0;
    if (current_shader == s) { return; }
    glUseProgram(s);
    current_shader = s;
}

void check_location(uint32_t location) {
    if (location == -1) {
        printf("shader location not found");
    }
}

void load_texture(Texture* t, const char* path) {
    int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0); 

	glGenTextures(1, t);
	glBindTexture(GL_TEXTURE_2D, *t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void bind_texture(const Texture t) {
    static Texture current_texture = 0;
    if (current_texture == t) { return; }
    glBindTexture(GL_TEXTURE_2D, t);
    current_texture = t;
}

void update_input_state() {
    memcpy(input.prev_keys, input.keys, MAX_KEYS);
    memcpy(input.prev_buttons, input.buttons, MAX_BUTTONS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= MAX_KEYS) return;
    input.keys[key] = (action != GLFW_RELEASE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button < 0 || button >= MAX_BUTTONS) return;
    input.buttons[button] = (action != GLFW_RELEASE);
}

bool first_mouse = true;
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

    if(first_mouse) {
        input.mouse_x = xpos;
        input.mouse_y = ypos;
        first_mouse = false;
    }

    input.mouse_delta_x = xpos - input.mouse_x;
    input.mouse_delta_y = input.mouse_y - ypos;

    input.mouse_x = xpos;
    input.mouse_y = ypos;
}

bool is_key_down(int key)       { return input.keys[key]; }
bool is_key_pressed(int key)    { return input.keys[key] && !input.prev_keys[key]; }
bool is_key_released(int key)   { return !input.keys[key] && input.prev_keys[key]; }

bool is_mouse_button_pressed(int btn) {
    return input.buttons[btn] && !input.prev_buttons[btn];
}

void print_fps() {
    static int frame = 0;
    if (++frame % 120 == 0) {
        printf("FPS: %.2f\n", 1.0f / delta_time);
    }
}