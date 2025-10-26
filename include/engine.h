

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    screen_has_resized = true;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= MAX_KEYS) return;
    input.keys[key] = (action != GLFW_RELEASE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button < 0 || button >= MAX_BUTTONS) return;
    input.buttons[button] = (action != GLFW_RELEASE);
}