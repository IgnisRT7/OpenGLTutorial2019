/**
*	@file main.cpp
*/

#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>
#include "SkeletalMesh.h"
#include "Audio.h"


#define GEFORCE_USED
#ifdef GEFORCE_USED

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

#endif
#include <iostream>

//エントリーポイント
int main() {

	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(1280,720,"title")) {
		return 0;
	}

	///音声再生プログラムを初期化する
	Audio::Engine& audioEngine = Audio::Engine::Instance();
	if (!audioEngine.Initialize()) {
		return 1;
	}

	//スケルタル・アニメーションを利用可能にする
	Mesh::SkeletalAnimation::Initialize();

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	
	float previousTime = glfwGetTime();

	//メインループ
	while(!window.ShouldClose()) {
		
		glClearColor(0.8f,0.2f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GLコンテキストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

//		window.UpdateDeltaTime();
		float nowTime = glfwGetTime();
		float deltaTime = nowTime - previousTime;
		previousTime = nowTime;

		window.UpdateGamePad();

		//ESCキーが押されたら終了ウインドウを表示
		if (window.KeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "ゲームを終了しますか?", "終了", MB_OKCANCEL) == IDOK) {
				break;
			}
		}
		
		//スケルタル・アニメーション用データの作成準備
		Mesh::SkeletalAnimation::ResetUniformData();

		sceneStack.Update(deltaTime);

		//スケルタル・アニメーション用データをGPUメモリに転送
		Mesh::SkeletalAnimation::UploadUniformData();

		//音声再生プログラムを更新する
		audioEngine.Update();

		sceneStack.Render();

		window.SwapBuffers();
	}

	//スケルタル・アニメーションの利用を終了する
	Mesh::SkeletalAnimation::Finalize();

	//音声再生プログラムを終了する
	audioEngine.Finalize();

	return 0;
}