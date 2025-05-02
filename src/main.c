#include "gl_backend.h"

int main() {

	gl_init();

	Window w;
	window_init(&w, "Test", 800, 600);

	glad_init();

	while (window_active(&w)) {
		start_frame();


		end_frame(&w);
	}

	return 0;
}