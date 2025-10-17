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

void begin_render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void end_render(GLFWwindow* window) {
    glfwSwapBuffers(window);
}
