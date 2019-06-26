/**
*	@file TitleScene.cpp
*/
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/**
*	�V�[��������������
*
*	@param true		����������
*	@param false	���������s
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
*	�V�[�����X�V����
*	
*	@param deltaTime	�O��̍X�V����̌o�ߎ���(�b)
*/
void TitleScene::Update(float deltaTime) {

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();
}


/**
*	�v���C���[�̓��͂���������
*/
void TitleScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (window.GetGamePad().buttonDown & GamePad::START) {

		SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
	}
}

/**
*	�V�[����`�悷��
*/
void TitleScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}


