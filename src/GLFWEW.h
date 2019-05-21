/**
*	@file GLFWEW.h
*/
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GamePad.h"
#include <map>
#include <vector>


namespace GLFWEW {

	/**
	*	GLFWとGLEWのラッパークラス
	*/
	class Window {
	public:
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;
		const GamePad& GetGamePad() const;
		void UpdateGamePad();

	private:

		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool isGLFWInitialized;
		bool isInitialized;
		GLFWwindow* window;
		GamePad gamepad;

	};



}	// namespace GLFWEW




