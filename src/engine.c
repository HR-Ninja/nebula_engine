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