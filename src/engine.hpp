#pragma once

#include<opengl.hpp>

namespace Engine {

	OpenGL& ctx = OpenGL::Instance();

	void Start() {
		ctx.SetWindow(1280, 720, "Test");
	}

	void Run() {

		while (!ctx.WindowShouldClose()) {
			ctx.StartFrame();



			ctx.EndFrame();
		}
	}

	void Exit() {
		ctx.Terminate();
	}

}



