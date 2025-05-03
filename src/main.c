#include "gl_backend.h"

const char* vertex_shader_src = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* frag_shader_src = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
    	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";


float vertices[] = {
	-0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

int main() {

	gl_init();

	Window w;
	window_init(&w, "Test", 800, 600);

	glad_init();

	uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
	glCompileShader(vertex_shader);

	uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
	glCompileShader(frag_shader);

	uint32_t shader = glCreateProgram();
	glAttachShader(shader, vertex_shader);
	glAttachShader(shader, frag_shader);
	glLinkProgram(shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	uint32_t VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  

	

	glUseProgram(shader);

	while (window_active(&w)) {
		start_frame();

		glDrawArrays(GL_TRIANGLES, 0, 3);

		end_frame(&w);
	}

	return 0;
}