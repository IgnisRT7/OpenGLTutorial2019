/**
*	@file TitleScene.cpp
*/
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/**
*	シーンを初期化する
*
*	@param true		初期化成功
*	@param false	初期化失敗
*
*/
bool TitleScene::Initialize() {

	spriteRenderer.Init(1000, "res/Sprite.vert", "res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::LoadFromFile("res/Player.dds"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	return true;
}

/**
*	シーンを更新する
*	
*	@param deltaTime	前回の更新からの経過時間(秒)
*/
void TitleScene::Update(float deltaTime) {

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();
}


/**
*	プレイヤーの入力を処理する
*/
void TitleScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (window.GetGamePad().buttonDown & GamePad::START) {

		SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
	}
}

/**
*	シーンを描画する
*/
void TitleScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}


