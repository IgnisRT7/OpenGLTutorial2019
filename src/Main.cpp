/**
*	@file main.cpp
*/

#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>

//エントリーポイント
int main() {

	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(1280,720,"title")) {
		return 0;
	}

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	while(!window.ShouldClose()) {

		//バックバッファを消去する TODO : コメントアウト対象
		
		glClearColor(0.8f,0.2f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GLコンテキストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		

		const float deltaTime = 1.0f / 60.0f;

		window.UpdateGamePad();

		// TODO : コメントアウト対象
		/*//ESCキーが押されたら終了ウインドウを表示
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "ゲームを終了しますか?", "終了", MB_OKCANCEL) == IDOK) {
				break;
			})
		}*/

		sceneStack.Update(deltaTime);
		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}