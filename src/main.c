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

typedef struct {
    vec3 pos;
    vec3 forward;
    vec3 up;
    float pitch;
    float yaw;
    float speed;
} CameraComponent;

void update_camera_system(float delta_time, Entity camera_entity, Component CameraComponentID);

int main() {

	engine_init("Test", 800, 600);

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

	Component CameraComponentID = register_component(sizeof(CameraComponent));
	Entity camera_entity = create_entity();

	CameraComponent cam = {
		.pos = {0.0f, 0.0f, 3.0f},
		.forward = {0.0f, 0.0f, -1.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.pitch = 0.0f,
		.yaw = -90.0f,
		.speed = 2.0f
	};

	attach_component(camera_entity, CameraComponentID, &cam);
	
	mat4 projection;
	glm_mat4_identity(projection);
	glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, projection);
	
	mat4 view;
	glm_mat4_identity(view);

	uint32_t view_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "view");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float*)view);

	uint32_t projection_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "projection");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const float*)projection);

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	while (window_active()) {
		start_frame();

		if(is_key_down(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

		update_camera_system(delta_time, camera_entity, CameraComponentID);

		// Rebuild view matrix
		CameraComponent* cam = get_component(camera_entity, CameraComponentID);

		vec3 target;
		glm_vec3_add(cam->pos, cam->forward, target);
		glm_lookat(cam->pos, target, cam->up, view);

		uint32_t view_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "view");
		glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float*)view);

		uint32_t model_location = glGetUniformLocation(shaders[SHADER_DEFAULT].id, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, (const float*)model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		end_frame();
	}

	return 0;
}

void update_camera_system(float delta_time, Entity camera_entity, Component CameraComponentID) {
    CameraComponent* cam = get_component(camera_entity, CameraComponentID);

    // Movement
    if (is_key_down(GLFW_KEY_W)) {
        vec3 move;
        glm_vec3_scale(cam->forward, cam->speed * delta_time, move);
        glm_vec3_add(cam->pos, move, cam->pos);
    }

    if (is_key_down(GLFW_KEY_S)) {
        vec3 move;
        glm_vec3_scale(cam->forward, cam->speed * delta_time, move);
        glm_vec3_sub(cam->pos, move, cam->pos);
    }

    // Mouse look
    cam->yaw   += input.mouse_delta_x * 0.1f;
    cam->pitch += input.mouse_delta_y * 0.1f;

    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;

    cam->forward[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    cam->forward[1] = sin(glm_rad(cam->pitch));
    cam->forward[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    glm_normalize(cam->forward);
}
