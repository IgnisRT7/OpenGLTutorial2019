/**
*	@file GLFWEW.cpp
*/
#include "GLFWEW.h"
#include <iostream>

///GLFW��GLEW�����b�v���邽�߂̖��O���
namespace GLFWEW {

	/**
	*	GLFW����̃G���[�񍐂���������
	*
	*	@param error	�G���[�̔ԍ�
	*	@param desc		�G���[�̓��e
	*/
	void ErrorCallback(int error, const char* desc) {

		std::cerr << "EEROR : " << desc << std::endl;
	}

	/**
	*	�V���O���g���C���X�^���X���擾����
	*
	*	@return Window�̃V���O���g���C���X�^���X
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	*	�R���X�g���N�^
	*/
	Window::Window() :
		isGLFWInitialized(false),
		isInitialized(false),
		window(nullptr) {

	}

	/**
	*	�f�X�g���N�^
	*/
	Window::~Window() {
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}

	/**
	*	GLFW/GLEW�̏�����
	*
	*	@param w �E�C���h�E�̕`��͈͂̕�(�s�N�Z��)
	*	@param h �E�C���h�E�̕`��͈͂̍���(�s�N�Z��)
	*	@param title �E�C���h�E�^�C�g��(UTF-8��0�I�[������)
	*
	*	@retval true ����������
	*	@retval false ���������s
	*/
	bool Window::Init(int w, int h, const char* title) {

		if (isInitialized) {
			std::cerr << "ERROR: GLFWEW�͊��ɏ���������Ă��܂�" << std::endl;
			return false;
		}

		if (!isGLFWInitialized) {
			glfwSetErrorCallback(ErrorCallback);

			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window) {
			window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window) {
				return false;
			}
			glfwMakeContextCurrent(window);
		}

		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂���" << std::endl;
			return false;
		}

		width = w;
		height = h;

		return true;
	}

	/**
	*	�E�C���h�E�����ׂ������ׂ�
	*
	*	retval true ����
	*	retval false ���Ȃ�
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	*	�t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	/**
	*	�Q�[���p�b�h�̏�Ԃ��擾����
	*
	*	@return �Q�[���p�b�h�̏��
	*/
	const GamePad& Window::GetGamePad() const {
		return gamepad;
	}

	/**
	*	�W���C�X�e�B�b�N�̃A�i���O���͑��uID
	*
	*	@note XBOX360�R���g���[���[�
	*/
	enum GAMEPAD_AXES {
		GAMEPAD_AXES_LEFT_X,	///< ���X�e�B�b�N��X��
		GAMEPAD_AXES_LEFT_Y,	///< ���X�e�B�b�N��Y��
		GAMEPAD_AXES_TRIGGER,	///< �A�i���O�g���K�[
		GAMEPAD_AXES_RIGHT_Y,	///< �E�X�e�B�b�N��X��
		GAMEPAD_AXES_RIGHT_X,	///< �E�X�e�B�b�N��Y��
	};

	/**
	*	�W���C�X�e�B�b�N�̃f�W�^�����͑��u
	*
	*	@note XBOX360�R���g���[���[�
	*/
	enum GAMEPAD_BUTTON{
		GAMEPAD_BUTTON_A,		///< A�{�^��
		GAMEPAD_BUTTON_B,		///< B�{�^��
		GAMEPAD_BUTTON_X,		///< X�{�^��
		GAMEPAD_BUTTON_Y,		///< Y�{�^��
		GAMEPAD_BUTTON_L,		///< L�{�^��
		GAMEPAD_BUTTON_R,		///< R�{�^��
		GAMEPAD_BUTTON_BACK,	///< Back�{�^��
		GAMEPAD_BUTTON_START,	///< Start�{�^��
		GAMEPAD_BUTTON_L_THUMB,///< ���X�e�B�b�N�{�^��
		GAMEPAD_BUTTON_R_THUMB,///< �E�X�e�B�b�N�{�^��
		GAMEPAD_BUTTON_UP	,	///< ��L�[
		GAMEPAD_BUTTON_RIGHT,	///< �E�L�[
		GAMEPAD_BUTTON_DOWN,	///< ���L�[
		GAMEPAD_BUTTON_LEFT,	///< ���L�[
	};

	/**
	*	�Q�[���p�b�h�̏�Ԃ��X�V����
	*/
	void Window::UpdateGamePad() {

		const uint32_t prevButtons = gamepad.buttons;
		int axesCount, buttonCount;

		//�����͂ƃ{�^�����͂̎擾
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		if (axes && buttons && axesCount >= 2 && buttonCount >= 8) {
			///�����͂̏���

			gamepad.buttons &= ~(GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT);

			static const float threshould = 0.3f;
			if (axes[GAMEPAD_AXES_LEFT_Y] > threshould) {
				gamepad.buttons |= GamePad::DPAD_UP;
			}
			else if (axes[GAMEPAD_AXES_LEFT_Y] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_DOWN;
			}
			if (axes[GAMEPAD_AXES_LEFT_X] > threshould) {
				gamepad.buttons |= GamePad::DPAD_LEFT;
			}
			else if (axes[GAMEPAD_AXES_LEFT_X] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_RIGHT;
			}


			static const struct {
				int dataIndex;
				uint32_t gamepadBit;
			} keyMap[] = {
				{ GAMEPAD_BUTTON_A, GamePad::A },
				{ GAMEPAD_BUTTON_B, GamePad::B },
				{ GAMEPAD_BUTTON_X, GamePad::X },
				{ GAMEPAD_BUTTON_Y, GamePad::Y },
				{ GAMEPAD_BUTTON_L, GamePad::L },
				{ GAMEPAD_BUTTON_R, GamePad::R },
				{ GAMEPAD_BUTTON_START, GamePad::START },
				{ GAMEPAD_BUTTON_UP, GamePad::DPAD_UP },
				{ GAMEPAD_BUTTON_DOWN, GamePad::DPAD_DOWN },
				{ GAMEPAD_BUTTON_LEFT, GamePad::DPAD_LEFT },
				{ GAMEPAD_BUTTON_RIGHT, GamePad::DPAD_RIGHT },

			};
			for (const auto& e : keyMap) {

				if (buttons[e.dataIndex] == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadBit;
				}
				else if (buttons[e.dataIndex] == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadBit;
				}
			}
		}
		else {
			//�L���ȃQ�[���p�b�h���ڑ�����Ă��Ȃ��̂ŁA�L�[�{�[�h���͂ő�p

			//�z��C���f�b�N�X��GamePad�L�[�̑Ή��\
			static const struct {
				int keyCode;
				uint32_t gamepadBit;
			}keyMap[] = {
				{ GLFW_KEY_J,GamePad::A },
				{ GLFW_KEY_K,GamePad::B },
				{ GLFW_KEY_U,GamePad::X }, 
				{ GLFW_KEY_I,GamePad::Y },
				{ GLFW_KEY_O,GamePad::L },
				{ GLFW_KEY_L,GamePad::R },
				{ GLFW_KEY_ENTER,GamePad::START },
				{ GLFW_KEY_W,GamePad::DPAD_UP },
				{ GLFW_KEY_A,GamePad::DPAD_DOWN },
				{ GLFW_KEY_S,GamePad::DPAD_LEFT },
				{ GLFW_KEY_D,GamePad::DPAD_RIGHT},			
			};

			for (const auto& e : keyMap) {

				const int key = glfwGetKey(window, e.keyCode);
				if (key == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadBit;
				}
				else if (key == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadBit;
				}
			}

		}
		
		//�����ꂽ�u�Ԃ̃f�[�^����
		gamepad.buttonDown = gamepad.buttons & ~prevButtons;
	}


} //namespace GLFWEW
