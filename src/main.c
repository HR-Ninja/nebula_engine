#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"

	float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

	unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

int main() {

	gl_init();
	window_init("testing", 800, 600);
	glad_init();

	load_shader(&shaders[SHADER_DEFAULT], "assets/vert.glsl", "assets/frag.glsl");
	use_shader(&shaders[SHADER_DEFAULT]);

	uint32_t VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);  

	Texture t;
	load_texture(&t, "assets/container.jpg");
	bind_texture(&t);

	mat4 model;
	glm_mat4_identity(model);
	//vec3 axis = {0.5f, 1.0f, 0.0f};
	
	Camera cam = {
		.pos = {0.0f, 0.0f, 3.0f},
		.forward = {0.0f, 0.0f, -1.0f},
		.up = {0.0f, 1.0f, 0.0f}
	};
	glm_mat4_identity(cam.view);	

	glm_mat4_identity(cam.projection);
	glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, cam.projection);
	

	uint32_t view_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "view");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float*)cam.view);

	uint32_t projection_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "projection");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const float*)cam.projection);

	glEnable(GL_DEPTH_TEST);

	while (window_active()) {
		start_frame();
		
		
		const float radius = 10.0f;
		float camX = (float)(sin(glfwGetTime()) * radius);
		float camZ = (float)(cos(glfwGetTime()) * radius);
		cam.pos[0] = camX;
		cam.pos[1] = 2.0f;
		cam.pos[2] = camZ;
		vec3 cam_target;
		glm_vec3_add(cam.pos, cam.forward, cam_target);
		glm_lookat(cam.pos, cam_target, cam.up, cam.view);

		uint32_t view_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "view");
		glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float*)cam.view);

		//glm_rotate(model, 2.0f * delta_time, axis);

		uint32_t model_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, (const float*)model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		end_frame();
	}

	return 0;
}