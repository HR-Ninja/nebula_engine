#define STB_IMAGE_IMPLEMENTATION
#include "engine2.h"

InputState g_input_state = {0};

float delta_time = 0.0f;
float last_time = 0.0f;
float accumulated_dt = 0.0f;

int screen_width = 800;
int screen_height = 600;
bool screen_has_resized = false;

bool engine_init(GLFWwindow* window, const char* title, uint32_t width, uint32_t height) {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    screen_width = width;
    screen_height = height;
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    
    if (window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
        printf("Failed to initialize GLAD");
		return false;
	}
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
    glViewport(0, 0, width, height);

    return true;
}

void start_frame() {
    float current_time = glfwGetTime();
    delta_time = current_time - last_time;
    accumulated_dt += delta_time;
    last_time = current_time;

    screen_has_resized = false;

	glfwPollEvents();
}

void end_frame() {
    g_input_state.mouse_delta_x = 0;
    g_input_state.mouse_delta_y = 0;
    update_input_state();
}

bool load_shader(Shader* s, const char* vert_src_path, const char* frag_src_path) {
    FILE* vert = fopen(vert_src_path, "rb");
    FILE* frag = fopen(frag_src_path, "rb");

    if(!vert || !frag) {
        perror("Failed to open file");
        return false;
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
        return false;
    }

    fread(vert_buffer, 1, vert_length, vert);
    vert_buffer[vert_length] = '\0';
    fclose(vert);

    fread(frag_buffer, 1, frag_length, frag);
    frag_buffer[frag_length] = '\0';
    fclose(frag);

    if (!compile_shader(s, vert_buffer, frag_buffer)) {
        free(vert_buffer);
        free(frag_buffer);
        return false;
    }

    free(vert_buffer);
    free(frag_buffer);

    return true;
}

bool compile_shader(Shader* s, const char* vertexSource, const char* fragmentSource) {
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
        return false;
    }


    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        fprintf(stderr, "Fragment shader compilation failed:\n%s\n", infoLog);
        return false;
    }

    *s = glCreateProgram();
    glAttachShader(*s, vertex);
    glAttachShader(*s, fragment);
    glLinkProgram(*s);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void use_shader(const Shader s) {
    static Shader current_shader = 0;
    if (current_shader == s) { return; }
    glUseProgram(s);
    current_shader = s;
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


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool first_mouse = true;
    if(first_mouse) {
        g_input_state.mouse_x = xpos;
        g_input_state.mouse_y = ypos;
        first_mouse = false;
    }

    g_input_state.mouse_delta_x = xpos - g_input_state.mouse_x;
    g_input_state.mouse_delta_y = g_input_state.mouse_y - ypos;

    g_input_state.mouse_x = xpos;
    g_input_state.mouse_y = ypos;
}

void print_fps() {
    static float avg = 0;
    avg = 0.9f * avg + 0.1f * delta_time;
    static int frame = 0;
    if (++frame % 60 == 0) {
        printf("FPS: %.2f\n", 1.0f / avg);
    }
}