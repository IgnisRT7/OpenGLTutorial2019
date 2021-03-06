/**
*	@file StatusScene.cpp
*/
#include "StatusScene.h"

/**
*	プレイヤーの入力を処理する
*/
bool StatusScene::Initialize(){

	return false;
}
void StatusScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad& gamepad = window.GetGamePad();

	if (gamepad.buttonDown & GamePad::START) {
		Audio::Engine::Instance().Prepare("Res/Audio/select.wav")->Play();
		SceneStack::Instance().Pop();
	}
}