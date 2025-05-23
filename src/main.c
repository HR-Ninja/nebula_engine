#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

typedef struct {

} PlayerTag;

typedef struct {
    vec3 position;
} Transform;

typedef struct {
    vec3 current;
    vec3 target;
} Velocity;

typedef struct {
    GLuint VAO;
    GLuint instanceVBO;
    Shader shader;
    Texture texture;
    uint32_t model_location;
} Sprite;

typedef enum {
    SHADER_DEFAULT,
    SHADER_TEST,
    SHADER_COUNT
} Shader_names;

typedef enum {
    NONE,
    CONTAINER,
    TEXTURE_COUNT
} Texture_names;

static Shader shaders[SHADER_COUNT];
static Texture textures[TEXTURE_COUNT];

static Component player_tag;

void init_sprite_data(Sprite *s, const Shader_names shader, const Texture_names texture);
void draw_system(Component sprite, Component transform);
void move_system(Component transform_c, Component velocity_c);
void player_controller(Entity e, Component velocity_c);
void scatter_system(Component transform_c, Component velocity_c);
void init_sprite_data2(Sprite *s, const Shader_names shader, const Texture_names texture);

void draw2(Component sprite_c, Component transform_c) {
    static mat4 instance_transforms[MAX_ENTITIES];
    size_t count = 0;

       Sprite *sprite = NULL;

    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        if (HAS_COMPONENT(entity_mask[e], transform_c) && HAS_COMPONENT(entity_mask[e], sprite_c)) {
            if (!sprite) sprite = (Sprite *)get_component(e, sprite_c);  // Assume all use same sprite

            Transform *pos = (Transform *)get_component(e, transform_c);

            glm_mat4_identity(instance_transforms[count]);
            glm_translate(instance_transforms[count], pos->position);
            glm_scale(instance_transforms[count], (vec3){100.0f, 100.0f, 1.0f});
            count++;
        }
    }

    if (count == 0 || !sprite) return;

    use_shader(sprite->shader);
    glActiveTexture(GL_TEXTURE0);
    bind_texture(sprite->texture);

    glBindVertexArray(sprite->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sprite->instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(mat4), instance_transforms);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count);

    glBindVertexArray(0);

}


int main() {
    
    engine_init("Test", 800, 600);
    
    load_shader(&shaders[SHADER_DEFAULT], "assets/2Dvert.glsl", "assets/2Dfrag.glsl");
    load_shader(&shaders[SHADER_TEST], "assets/2Dvert2.glsl", "assets/2Dfrag.glsl");
    
    load_texture(&textures[CONTAINER], "assets/container.jpg");
    
    mat4 projection;
    glm_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f, projection);
    use_shader(shaders[SHADER_DEFAULT]);
    uint32_t location = glGetUniformLocation(shaders[SHADER_DEFAULT], "projection");
    check_location(location);
    glUniformMatrix4fv(location, 1, false, (float *)projection);
    
    location = glGetUniformLocation(shaders[SHADER_DEFAULT], "image");
    check_location(location);
    glUniform1i(location, 0);

    use_shader(shaders[SHADER_TEST]);
    location = glGetUniformLocation(shaders[SHADER_TEST], "projection");
    check_location(location);
    glUniformMatrix4fv(location, 1, false, (float *)projection);

    location = glGetUniformLocation(shaders[SHADER_TEST], "image");
    check_location(location);
    glUniform1i(location, 0);

    
    player_tag = register_component(sizeof(PlayerTag));
    Component sprite_component = register_component(sizeof(Sprite));
    Component transform_component = register_component(sizeof(Transform));
    Component velocity_component = register_component(sizeof(Velocity));

    Entity player = create_entity();

    Sprite s;
    init_sprite_data(&s, SHADER_DEFAULT, CONTAINER);    

    Sprite s2;
    init_sprite_data2(&s2, SHADER_TEST, CONTAINER);   
    
    Transform p = {
        .position = {300.0f, 300.0f, 0.0f}
    };
    
    Transform p2 = {
        .position = {700.0f, 300.0f, 0.0f}
    };
    
    Velocity v = {
        .current = {0.0f, 0.0f, 0.0f},
        .target = {0.0f, 0.0f, 0.0f}
    };

    PlayerTag tag;
    attach_component(player, player_tag, &tag);
    //attach_component(player, sprite_component, &s);
    attach_component(player, transform_component, &p);
    attach_component(player, velocity_component, &v);

    for (int i = 0; i < 100; i++) {
        Entity e = create_entity();
        attach_component(e, sprite_component, &s2);

        Transform p = {
        .position = {i + 0.1f, i + 0.1f, 0.0f}
        };
        attach_component(e, transform_component, &p);
        attach_component(e, velocity_component, &v);
    };

	while (window_active()) {
		start_frame();

		if(is_key_down(GLFW_KEY_ESCAPE)) {
		    glfwSetWindowShouldClose(window, GLFW_TRUE);
	    }

        scatter_system(transform_component, velocity_component);
        player_controller(player, velocity_component);
        move_system(transform_component, velocity_component);
        //draw_system(sprite_component, transform_component);
        draw2(sprite_component, transform_component);

        print_fps();

		end_frame();
	}

    free_components();

	return 0;
}

void scatter_system(Component transform_c, Component velocity_c) {

    const float scatter_speed = 100.0f;
    const float screen_width = 800.0f;
    const float screen_height = 600.0f;

    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        if (HAS_COMPONENT(entity_mask[e], player_tag)) { continue; };

        if (HAS_COMPONENT(entity_mask[e], transform_c) && HAS_COMPONENT(entity_mask[e], velocity_c)) {
            Transform *pos = (Transform *)get_component(e, transform_c);
            Velocity *vel = (Velocity *)get_component(e, velocity_c);

            if (glm_vec3_norm(vel->target) == 0.0f) {
                float dir_x = ((float)(rand() % 201) - 100.0f) / 100.0f;  // Range: -1.0 to 1.0
                float dir_y = ((float)(rand() % 201) - 100.0f) / 100.0f;
                vec3 dir = {dir_x, dir_y, 0.0f};
                glm_vec3_normalize(dir);
                glm_vec3_scale(dir, scatter_speed, vel->target);
            }

            // Rebound on edges
            if (pos->position[0] <= 0.0f || pos->position[0] >= screen_width) {
                vel->target[0] *= -1.0f;
                pos->position[0] = CLAMP(pos->position[0], 0.0f, screen_width);
            }

            if (pos->position[1] <= 0.0f || pos->position[1] >= screen_height) {
                vel->target[1] *= -1.0f;
                pos->position[1] = CLAMP(pos->position[1], 0.0f, screen_height);
            }
            

        }
    }
}

void move_system(Component transform_c, Component velocity_c) {
    const float acceleration = 10.0f;

    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        if (HAS_COMPONENT(entity_mask[e], transform_c) && HAS_COMPONENT(entity_mask[e], velocity_c)) {
            Transform *pos = (Transform *)get_component(e, transform_c);
            Velocity *vel = (Velocity *)get_component(e, velocity_c);

            // Lerp current velocity toward target velocity
            glm_vec3_lerp(vel->current, vel->target, acceleration * delta_time, vel->current);

            // Calculate delta position
            vec3 delta_pos;
            glm_vec3_scale(vel->current, delta_time, delta_pos);
            glm_vec3_add(pos->position, delta_pos, pos->position);

        }
    }
}


void draw_system(Component sprite_c, Component transform_c) {
    mat4 model;

    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        if (HAS_COMPONENT(entity_mask[e], transform_c) && HAS_COMPONENT(entity_mask[e], sprite_c)) {
            Sprite *sprite = (Sprite *)get_component(e, sprite_c);
            Transform *pos = (Transform *)get_component(e, transform_c);

            glm_mat4_identity(model);

            glm_translate(model, pos->position);
            glm_scale(model, (vec3){100.0f, 100.0f, 1.0f});

            use_shader(sprite->shader);
            check_location(sprite->model_location);
            glUniformMatrix4fv(sprite->model_location, 1, false, (float *)model);
            
            glActiveTexture(GL_TEXTURE0);
            bind_texture(sprite->texture);

            glBindVertexArray(sprite->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
}

void init_sprite_data(Sprite *s, const Shader_names shader, const Texture_names texture) {
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &s->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(s->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    s->shader = shaders[shader];
    s->model_location = glGetUniformLocation(s->shader, "model");
    check_location(s->model_location);

    if (texture != NONE) {
        s->texture = textures[texture];
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);
}

void init_sprite_data2(Sprite *s, const Shader_names shader, const Texture_names texture) {
    GLuint VBO, instanceVBO;

    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &s->VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(s->VAO);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // Instance matrix
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_ENTITIES * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);

    GLsizei vec4Size = sizeof(vec4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const void *)(uintptr_t)(i * vec4Size));
        glVertexAttribDivisor(1 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    s->instanceVBO = instanceVBO;

    s->shader = shaders[shader];

    if (texture != NONE) {
        s->texture = textures[texture];
    }
}


void player_controller(Entity e, Component velocity_c) {
    if(HAS_COMPONENT(entity_mask[e], velocity_c)) {
        Velocity *vel = (Velocity *)get_component(e, velocity_c);

        glm_vec3_zero(vel->target);

        if (is_key_down(GLFW_KEY_W)) {
            vel->target[1] -= 1.0f;
        }
        if (is_key_down(GLFW_KEY_S)) {
            vel->target[1] += 1.0f;
        }
        if (is_key_down(GLFW_KEY_A)) {
            vel->target[0] -= 1.0f;
        }
        if (is_key_down(GLFW_KEY_D)) {
            vel->target[0] += 1.0f;
        }

        // Normalize and scale to speed if any movement was pressed
        if (glm_vec3_norm(vel->target) > 0.0f) {
            glm_vec3_normalize(vel->target);
            const float speed = 200.0f;
            glm_vec3_scale(vel->target, speed, vel->target);
        }
    }
}