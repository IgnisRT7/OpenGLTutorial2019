/**
*	@file main.cpp
*/

#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>
#include "SkeletalMesh.h"

//エントリーポイント
int main() {

	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(1280,720,"title")) {
		return 0;
	}

	//スケルタル・アニメーションを利用可能にする
	Mesh::SkeletalAnimation::Initialize();

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	while(!window.ShouldClose()) {
		
		glClearColor(0.8f,0.2f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GLコンテキストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		window.UpdateDeltaTime();

		window.UpdateGamePad();

		//ESCキーが押されたら終了ウインドウを表示
		if (window.KeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "ゲームを終了しますか?", "終了", MB_OKCANCEL) == IDOK) {
				break;
			}
		}
		
		//スケルタル・アニメーション用データの作成準備
		Mesh::SkeletalAnimation::ResetUniformData();

		sceneStack.Update(window.DeltaTime());

		//スケルタル・アニメーション用データをGPUメモリに転送
		Mesh::SkeletalAnimation::UploadUniformData();

		sceneStack.Render();

		window.SwapBuffers();
	}

	//スケルタル・アニメーションの利用を終了する
	Mesh::SkeletalAnimation::Finalize();

	return 0;
}