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

//�G���g���[�|�C���g
int main() {

	auto& window = GLFWEW::Window::Instance();

	if (!window.Init(1280,720,"title")) {
		return 0;
	}

	///�����Đ��v���O����������������
	Audio::Engine& audioEngine = Audio::Engine::Instance();
	if (!audioEngine.Initialize()) {
		return 1;
	}

	//�X�P���^���E�A�j���[�V�����𗘗p�\�ɂ���
	Mesh::SkeletalAnimation::Initialize();

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	
	float previousTime = glfwGetTime();

	//���C�����[�v
	while(!window.ShouldClose()) {
		
		glClearColor(0.8f,0.2f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GL�R���e�L�X�g�̃p�����[�^��ݒ�
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

//		window.UpdateDeltaTime();
		float nowTime = glfwGetTime();
		float deltaTime = nowTime - previousTime;
		previousTime = nowTime;

		window.UpdateGamePad();

		//ESC�L�[�������ꂽ��I���E�C���h�E��\��
		if (window.KeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "�Q�[�����I�����܂���?", "�I��", MB_OKCANCEL) == IDOK) {
				break;
			}
		}
		
		//�X�P���^���E�A�j���[�V�����p�f�[�^�̍쐬����
		Mesh::SkeletalAnimation::ResetUniformData();

		sceneStack.Update(deltaTime);

		//�X�P���^���E�A�j���[�V�����p�f�[�^��GPU�������ɓ]��
		Mesh::SkeletalAnimation::UploadUniformData();

		//�����Đ��v���O�������X�V����
		audioEngine.Update();

		sceneStack.Render();

		window.SwapBuffers();
	}

	//�X�P���^���E�A�j���[�V�����̗��p���I������
	Mesh::SkeletalAnimation::Finalize();

	//�����Đ��v���O�������I������
	audioEngine.Finalize();

	return 0;
}