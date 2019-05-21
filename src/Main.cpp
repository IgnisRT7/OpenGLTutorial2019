/**
*	@file main.cpp
*/

#include "TitleScene.h"
#include "GLFWEW.h"

//エントリーポイント
int main() {

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"title")) {
		return 0;
	}

	for (;!window.ShouldClose();) {

		const float deltaTime = 1.0f / 60.0f;

		window.UpdateGamePad();

		sceneStack.Update(deltaTime);
		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}