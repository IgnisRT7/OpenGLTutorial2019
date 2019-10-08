/**
*	@file GameOverScene.cpp
*/
#include "GameOverScene.h"
#include "TitleScene.h"

/**
*	ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ðˆ—‚·‚é
*/
bool GameOverScene::Initialize(){

	bgm = Audio::Engine::Instance().Prepare("Res/Audio/gameover.wav");
	bgm->Play(Audio::Flag_Loop);
	return false;
}
void GameOverScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad& gamepad = window.GetGamePad();

	if (gamepad.buttonDown&GamePad::START) {

		SceneStack::Instance().Replace(std::make_shared<TitleScene>());
	}

}

void GameOverScene::Finalize(){

	bgm->Stop();
}
