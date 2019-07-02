/**
 * @file GLFWEW.cpp
 */

#include "GLFWEW.h"
#include <iostream>

// GLFW と GLEW をラップするための名前空間
namespace GLFWEW{
	
/**
 * GLFW からのエラー報告を処理する 
 * 
 * @param error	エラー番号　
 * @param desc	エラーの内容
 */
void ErrorCallback(int error, const char* desc){
	std::cerr << "ERROR : " << desc << std::endl;

}

/**
 * シングルトンインスタンスを取得する
 * 
 * @return Windowのシングルトンインスタンス
 */
Window& Window::Instance(){
	static Window instance;
	return instance;
}

/**
 * コンストラクタ : メンバ変数の初期化のみ
 */
Window::Window() : isGLFWInitialized(false), isInitialized(false), window(nullptr)
{
}

/**
 * デストラクタ : GLFWが初期化されていた場合のみ、呼び出される
 */
Window::~Window(){
	if(isGLFWInitialized){
		// GLFWを終了させる
		glfwTerminate();
	}
}

/**
 * GLFW / GLEW の初期化
 * 
 * @param w	ウィンドウの描画範囲の幅(ピクセル)
 * @param h	ウィンドウの描画範囲の高さ(ピクセル)
 * @param title	ウィンドウタイトル(UTF-8の0終端文字列)
 * 
 * @retval true		初期化成功
 * @retval false	初期化失敗
 */

bool Window::Init(int w, int h, const char* title){
	
	if(isInitialized){
		std::cerr << "ERROR: GLFWEWは既に初期化されています." << std::endl;
		return false;
	
	}

	// GLFWの初期化
	if(!isGLFWInitialized){
		glfwSetErrorCallback(ErrorCallback);
		if(glfwInit() != GL_TRUE){
			return false;
		}
		isGLFWInitialized = true;
	}

	// グラフィック描画用のウィンドウを作成
	if(!window){
		window = glfwCreateWindow(w, h, title, nullptr, nullptr);
		if(!window){
			return false;
		}
		// 指定したウィンドウに対応するOpenGLコンテキストを描画対象に設定
		glfwMakeContextCurrent(window);		
	}

	// GLEWの初期化
	if(glewInit() != GLEW_OK){
		std::cerr << "ERROR: GLEWの初期化に失敗しました" << std::endl;
		return false;
	}

	// 描画デバイスと対応バージョンをコンソールウィンドウに出力する。
	const GLubyte* renderer = glGetString(GL_RENDERER);
	std::cout << "Renderer: " << renderer << std::endl;
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Version: " << version << std::endl;

}

/**
 * ウィンドウを閉じるべきか調べる
 * 
 * @retval true 閉じる
 * @retval false 閉じない
 */
bool Window::ShouldClose() const{

	// 終了要求->0以外 ,　終了要求が来ていなけれ->0　
	return glfwWindowShouldClose(window) != 0;
}

/**
 * フロントバッファとバックバッファを切り替える
 */

void Window::SwapBuffers() const{

	// GLFWがOSから送られたイベントを処理
	glfwPollEvents();
	// フレームバッファの表示側と描画側を入れ替える
	glfwSwapBuffers(window);	
}

}	// namespace GLFWEW