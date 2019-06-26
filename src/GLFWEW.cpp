/**
*	@file GLFWEW.cpp
*/
#include "GLFWEW.h"
#include <iostream>

///GLFWとGLEWをラップするための名前空間
namespace GLFWEW {

	/**
	*	GLFWからのエラー報告を処理する
	*
	*	@param error	エラーの番号
	*	@param desc		エラーの内容
	*/
	void ErrorCallback(int error, const char* desc) {

		std::cerr << "EEROR : " << desc << std::endl;
	}

	/**
	*	シングルトンインスタンスを取得する
	*
	*	@return Windowのシングルトンインスタンス
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	*	コンストラクタ
	*/
	Window::Window() :
		isGLFWInitialized(false),
		isInitialized(false),
		window(nullptr) {

	}

	/**
	*	デストラクタ
	*/
	Window::~Window() {
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}

	/**
	*	GLFW/GLEWの初期化
	*
	*	@param w ウインドウの描画範囲の幅(ピクセル)
	*	@param h ウインドウの描画範囲の高さ(ピクセル)
	*	@param title ウインドウタイトル(UTF-8の0終端文字列)
	*
	*	@retval true 初期化成功
	*	@retval false 初期化失敗
	*/
	bool Window::Init(int w, int h, const char* title) {

		if (isInitialized) {
			std::cerr << "ERROR: GLFWEWは既に初期化されています" << std::endl;
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
			std::cerr << "ERROR: GLEWの初期化に失敗しました" << std::endl;
			return false;
		}

		width = w;
		height = h;

		return true;
	}

	/**
	*	ウインドウを閉じるべきか調べる
	*
	*	retval true 閉じる
	*	retval false 閉じない
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	*	フロントバッファとバックバッファを切り替える
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	/**
	*	ゲームパッドの状態を取得する
	*
	*	@return ゲームパッドの状態
	*/
	const GamePad& Window::GetGamePad() const {
		return gamepad;
	}

	/**
	*	ジョイスティックのアナログ入力装置ID
	*
	*	@note XBOX360コントローラー基準
	*/
	enum GAMEPAD_AXES {
		GAMEPAD_AXES_LEFT_X,	///< 左スティックのX軸
		GAMEPAD_AXES_LEFT_Y,	///< 左スティックのY軸
		GAMEPAD_AXES_TRIGGER,	///< アナログトリガー
		GAMEPAD_AXES_RIGHT_Y,	///< 右スティックのX軸
		GAMEPAD_AXES_RIGHT_X,	///< 右スティックのY軸
	};

	/**
	*	ジョイスティックのデジタル入力装置
	*
	*	@note XBOX360コントローラー基準
	*/
	enum GAMEPAD_BUTTON{
		GAMEPAD_BUTTON_A,		///< Aボタン
		GAMEPAD_BUTTON_B,		///< Bボタン
		GAMEPAD_BUTTON_X,		///< Xボタン
		GAMEPAD_BUTTON_Y,		///< Yボタン
		GAMEPAD_BUTTON_L,		///< Lボタン
		GAMEPAD_BUTTON_R,		///< Rボタン
		GAMEPAD_BUTTON_BACK,	///< Backボタン
		GAMEPAD_BUTTON_START,	///< Startボタン
		GAMEPAD_BUTTON_L_THUMB,///< 左スティックボタン
		GAMEPAD_BUTTON_R_THUMB,///< 右スティックボタン
		GAMEPAD_BUTTON_UP	,	///< 上キー
		GAMEPAD_BUTTON_RIGHT,	///< 右キー
		GAMEPAD_BUTTON_DOWN,	///< 下キー
		GAMEPAD_BUTTON_LEFT,	///< 左キー
	};

	/**
	*	ゲームパッドの状態を更新する
	*/
	void Window::UpdateGamePad() {

		const uint32_t prevButtons = gamepad.buttons;
		int axesCount, buttonCount;

		//軸入力とボタン入力の取得
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		if (axes && buttons && axesCount >= 2 && buttonCount >= 8) {
			///軸入力の処理

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
			//有効なゲームパッドが接続されていないので、キーボード入力で代用

			//配列インデックスとGamePadキーの対応表
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
		
		//押された瞬間のデータ入力
		gamepad.buttonDown = gamepad.buttons & ~prevButtons;
	}


} //namespace GLFWEW
