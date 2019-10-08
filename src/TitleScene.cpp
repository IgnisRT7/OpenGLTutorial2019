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
	Sprite spr(Texture::Image2D::Create("res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	//BGM���Đ�����
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/title2.wav");
	bgm->Play(Audio::Flag_Loop);

	return true;
}

/**
*	�V�[�����X�V����
*	
*	@param deltaTime	�O��̍X�V����̌o�ߎ���(�b)
*/
void TitleScene::Update(float deltaTime) {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const float w = window.Width();
	const float h = window.Height();
	const float lineHeight = fontRenderer.LineHeight();
	fontRenderer.BeginUpdate();
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h*0.5f - lineHeight), L"�^�C�g�����");
	fontRenderer.AddString(glm::vec2(-128, 0), L"�A�N�V�����Q�[��");
	fontRenderer.EndUpdate();

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();

	//�V�[���؂�ւ��҂�
	if (timer > 0) {
		timer -= deltaTime;
		if (timer <= 0) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
			return;
		}
	}
}


/**
*	�v���C���[�̓��͂���������
*/
void TitleScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (window.GetGamePad().buttonDown & GamePad::START) {

		//TODO: ���炭Prepare�ł͂Ȃ�Play�֐��Ăяo��
		//SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
		Audio::Engine::Instance().Prepare("Res/Audio/select2.wav")->Play();
		timer = 1.0f;
	}
}

/**
*	�V�[����`�悷��
*/
void TitleScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);
}


