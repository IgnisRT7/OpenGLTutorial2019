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

		int Width() const { return width; }
		int Height() const { return height; }

	private:

		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool isGLFWInitialized = false;
		bool isInitialized = false;
		GLFWwindow* window = nullptr;
		GamePad gamepad;

		int width = 0;
		int height = 0;
	};



}	// namespace GLFWEW




