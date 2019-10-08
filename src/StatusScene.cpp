/**
*	@file StatusScene.cpp
*/
#include "StatusScene.h"

/**
*	ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ðˆ—‚·‚é
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