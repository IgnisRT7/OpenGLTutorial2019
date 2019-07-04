/**
*	@file MainGameScene.cpp
*/

#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>

/**
*	初期化処理
*/
bool MainGameScene::Initialize(){

	spriteRenderer.Init(1000, "res/Sprite.vert", "res/Sprite.frag");
	fontRenderer.Init(1000);
	sprites.reserve(100);

	fontRenderer.LoadFromFile("Res/font.fnt");
	Sprite spr(Texture::Image2D::Create("res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));

	//ハイトマップを作成する
	if (!heightMap.LoadFromFile("Res/Terrain2.tga", 20.0f, 0.5f)) {
		return false;
	}
	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}

	return true;
}

/**
*	プレイヤーの入力を処理する
*/
void MainGameScene::ProcessInput() {

	if (!frag) {
		frag = true;
	//	SceneStack::Instance().Push(std::make_shared<StatusScene>());
	}
	else {

	//	SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
	}
}

/**
*	更新処理
*
*	@param deltaTime	経過時間
*/
void MainGameScene::Update(float deltaTime){


}

/**
*	描画処理
*/
void MainGameScene::Render(){

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);

	const glm::vec3 targetPos(100, 0, 100);
	const glm::vec3 cameraPos = targetPos + glm::vec3(0, 50, 50);
	const glm::mat4 matView = glm::lookAt(cameraPos, targetPos, glm::vec3(0, 1, 0));

	const float aspectRatio = static_cast<float>(window.Width() / static_cast<float>(window.Height()));
	const glm::mat4 matProj = glm::perspective(glm::radians(30.0f), aspectRatio, 1.0f, 1000.0f);
	const glm::mat4 matModel(1);
	Mesh::Draw(meshBuffer.GetFile("Cube"), matProj*matView, matModel);
	Mesh::Draw(meshBuffer.GetFile("Terrain"), matProj*matView, glm::mat4(1));

}
