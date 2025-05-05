#pragma once
#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include "stb_image.h"
#include "cglm/cglm.h"
#include"stdio.h"
#include <stdlib.h>

static GLFWwindow* window;
static float last_time;
static float delta_time;

typedef struct {
    uint32_t id;
} Shader;

typedef struct {
    uint32_t texture;
} Texture;

// Declerations
// Core
void gl_init();
void glad_init();
void window_init(const char* title, uint32_t width, uint32_t height);
void start_frame();
void end_frame();
void check_window();
int window_active();

// Shaders
Shader load_shader(const char* vert_src_path, const char* frag_src_path);
void compile_shader(Shader* s, const char* vertexSource, const char* fragmentSource);
void use_shader(Shader* s);

// Textures
Texture load_texture(const char* path);
void bind_texture(const Texture* t);


// Implementations
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

void window_init(const char* title, uint32_t width, uint32_t height) {

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	check_window();

	glfwMakeContextCurrent(window);
}

void start_frame() {
    float current_time = glfwGetTime();
    delta_time = current_time - last_time;
    last_time = current_time;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
}

void end_frame() {
	glfwSwapBuffers(window);
}

int window_active() {
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

Shader load_shader(const char* vert_src_path, const char* frag_src_path) {
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

    Shader s;
    compile_shader(&s, vert_buffer, frag_buffer);
    free(vert_buffer);
    free(frag_buffer);
    return s;
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

    s->id = glCreateProgram();
    glAttachShader(s->id, vertex);
    glAttachShader(s->id, fragment);
    glLinkProgram(s->id);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void use_shader(Shader* s) {
    glUseProgram(s->id);
}

Texture load_texture(const char* path) {
    int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0); 

    Texture t;
	glGenTextures(1, &t.texture);
	glBindTexture(GL_TEXTURE_2D, t.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
    return t;
}

void bind_texture(const Texture* t) {
    glBindTexture(GL_TEXTURE_2D, t->texture);
}