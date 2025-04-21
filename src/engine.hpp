#pragma once

#include<opengl.hpp>

namespace Engine {

	void Start() {
		OpenGL::Instance().SetWindow(1280, 720, "Test");
	}

	void Run() {

		OpenGL& ctx = OpenGL::Instance();

		while (!ctx.WindowShouldClose()) {

		}
	}


}



