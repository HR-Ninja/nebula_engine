

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
    uint32_t vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", infoLog);
    }


    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        fprintf(stderr, "Fragment shader compilation failed:\n%s\n", infoLog);
    }

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
    if (location == (uint32_t)-1) {
        printf("shader location not found\n");
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
	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    screen_has_resized = true;
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
    static float avg = 0;
    avg = 0.9f * avg + 0.1f * delta_time;
    static int frame = 0;
    if (++frame % 60 == 0) {
        printf("FPS: %.2f\n", 1.0f / avg);
    }
}
