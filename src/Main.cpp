/**
*	@file main.cpp
*/

#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>

//�G���g���[�|�C���g
int main() {

	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(1280,720,"title")) {
		return 0;
	}

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	while(!window.ShouldClose()) {

		//�o�b�N�o�b�t�@���������� TODO : �R�����g�A�E�g�Ώ�
		
		glClearColor(0.8f,0.2f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GL�R���e�L�X�g�̃p�����[�^��ݒ�
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		

		const float deltaTime = 1.0f / 60.0f;

		window.UpdateGamePad();

		// TODO : �R�����g�A�E�g�Ώ�
		/*//ESC�L�[�������ꂽ��I���E�C���h�E��\��
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "�Q�[�����I�����܂���?", "�I��", MB_OKCANCEL) == IDOK) {
				break;
			})
		}*/

		sceneStack.Update(deltaTime);
		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}