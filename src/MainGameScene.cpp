/**
*	@file MainGameScene.cpp
*/

#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <random>

/**
*	衝突を解決する
*
*	@param a	衝突したアクターその１
*	@param b	衝突したアクターその２
*	@param p	衝突位置
*/
void PlayerCollisionHandler(const ActorPtr& a, const ActorPtr& b, const glm::vec3& p) {

	//const glm::vec3 v = a->colWorld.center - p;
	const glm::vec3 v = a->colWorld.s.center - p;
	//衝突位置との距離が近すぎないか調べる
	if (dot(v, v) > FLT_EPSILON) {
		//aをbに重ならない位置まで移動
		const glm::vec3 vn = normalize(v);
		//const float radiusSum = a->colWorld.r + b->colWorld.r;
		float radiusSum = a->colWorld.s.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;
		}

		const float distance = radiusSum - glm::length(v) + 0.01f;
		a->position += vn * distance;

		//a->colWorld.center += vn * distance;
		a->colWorld.s.center += vn * distance;
	}
	else {
		//移動を取り消す(距離が近すぎる場合の例外処理)
		const float dletaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity;
		a->colWorld.s.center -= deltaVelocity;
	}
}

/**
*	初期化処理
*/
bool MainGameScene::Initialize() {

	spriteRenderer.Init(1000, "res/Sprite.vert", "res/Sprite.frag");
	fontRenderer.Init(1000);
	sprites.reserve(100);

	fontRenderer.LoadFromFile("Res/font.fnt");
	Sprite spr(Texture::Image2D::Create("res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	//メッシュバッファの初期化処理
	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));
	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadMesh("Res/bikuni.gltf");
	meshBuffer.LoadMesh("Res/oni_small.gltf");
	meshBuffer.LoadMesh("Res/TestTree.gltf");
	meshBuffer.LoadMesh("Res/wall_stone.gltf");

	//ハイトマップを作成する
	if (!heightMap.LoadFromFile("Res/Terrain3.tga", 20.0f, 0.5f)) {
		return false;
	}
	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}

	//プレイヤーの作成処理
	glm::vec3 startPos(100, 0, 100);
	startPos.y = heightMap.Height(startPos);
	player = std::make_shared<StaticMeshActor>(meshBuffer.GetFile("Res/bikuni.gltf"), "Player", 20, startPos);
	//player->colLocal = Collision::Sphere(glm::vec3(0), 0.5f);
	player->colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);

	SpawnKooni(50);
	SpawnTree(1);
	CreateStoneWall(startPos);

	return true;
}

/**
*	プレイヤーの入力を処理する
*/
void MainGameScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = window.GetGamePad();

	//プレイヤー操作
	glm::vec3 velocity(0);
	if (gamepad.buttons&GamePad::DPAD_LEFT) {
		velocity.x = -1;
	}
	else if (gamepad.buttons&GamePad::DPAD_RIGHT) {
		velocity.x = 1;
	}
	if (gamepad.buttons&GamePad::DPAD_DOWN) {
		velocity.z = 1;
	}
	else if (gamepad.buttons&GamePad::DPAD_UP) {
		velocity.z = -1;
	}
	if (velocity.x || velocity.z) {
		velocity = glm::normalize(velocity);
		player->rotation.y = std::atan2(-velocity.z, velocity.x) + glm::radians(90.0f);
		velocity *= 6.0f;
	}
	//	camera.velocity = velocity;
	player->velocity = velocity;

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
void MainGameScene::Update(float deltaTime) {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	{
		camera.target = player->position;
		camera.position = camera.target + glm::vec3(0, 50, 50);
	}

	player->Update(deltaTime);
	enemies.Update(deltaTime);
	trees.Update(deltaTime);
	objects.Update(deltaTime);

	player->position.y = heightMap.Height(player->position);
	DetectCollision(player, enemies, PlayerCollisionHandler);
	DetectCollision(player, trees, PlayerCollisionHandler);
	player->position.y = heightMap.Height(player->position);

	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);
	trees.UpdateDrawData(deltaTime);
	objects.UpdateDrawData(deltaTime);

	const float w = static_cast<float>(window.Width());
	const float h = static_cast<float>(window.Height());
	const float lineHeight = fontRenderer.LineHeight();
	fontRenderer.BeginUpdate();
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h*0.5f - lineHeight), L"メインゲーム画面");
	fontRenderer.EndUpdate();
}

/**
*	描画処理
*/
void MainGameScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);


	const glm::mat4 matView = glm::lookAt(camera.position, camera.target, camera.up);

	const float aspectRatio = static_cast<float>(window.Width() / static_cast<float>(window.Height()));
	const glm::mat4 matProj = glm::perspective(glm::radians(30.0f), aspectRatio, 1.0f, 1000.0f);

	glm::vec3 cubePos(120, 0, 120);
	cubePos.y = heightMap.Height(cubePos);
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);
	meshBuffer.SetViewProjectionMatrix(matProj *matView);

	Mesh::Draw(meshBuffer.GetFile("Cube"), matModel);
	Mesh::Draw(meshBuffer.GetFile("Terrain"), glm::mat4(1));

	glm::vec3 treePos(110, 0, 110);
	treePos.y = heightMap.Height(treePos);
	const glm::mat4 matTreeModel = glm::translate(glm::mat4(1), treePos) * glm::scale(glm::mat4(1), glm::vec3(3));
	Mesh::Draw(meshBuffer.GetFile("Res/red_pine_tree.gltf"), matTreeModel);

	player->Draw();
	enemies.Draw();
	trees.Draw();
	objects.Draw();
}

/**
*	小鬼のスポーン処理
*
*	@param n	スポーン数
*/
void MainGameScene::SpawnKooni(int n) {
	
	std::mt19937 rand;
	rand.seed(0);

	//敵を配置
	const size_t oniCount = n;
	enemies.Reserve(oniCount);
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/oni_small.gltf");
	for (size_t i = 0; i < oniCount; ++i) {
		//敵の位置を(50,50)-(150,150)の範囲からランダムに選択
		glm::vec3 position(0);
		position.x = std::uniform_real_distribution<float>(50, 150)(rand);
		position.z = std::uniform_real_distribution<float>(50, 150)(rand);
		position.y = heightMap.Height(position);

		//敵の向きをランダムに選択
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(rand);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(mesh, "Kooni", 13, position, rotation);
		//p->colLocal = Collision::Sphere(glm::vec3(0), 1.0f);
		p->colLocal = Collision::CreateCapsule(glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
		enemies.Add(p);
	}
}

/**
*	木のスポーン処理
*
*	@param n	スポーン数
*/
void MainGameScene::SpawnTree(int n) {
	
	std::mt19937 rand;
	rand.seed(0);

	//木を配置
	const size_t treeCount = n;
	trees.Reserve(treeCount);
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/TestTree.gltf");
	for (size_t i = 0; i < treeCount; ++i) {
		//木の位置を(50,50)-(150,150)の範囲からランダムに選択
		glm::vec3 position(0);
		position.x = std::uniform_real_distribution<float>(50, 150)(rand);
		position.z = std::uniform_real_distribution<float>(50, 150)(rand);
		position.y = heightMap.Height(position);

		//木の向きをランダムに選択
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(rand);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(mesh, "Tree", 13, position, rotation);
		//p->colLocal = Collision::Sphere(glm::vec3(0), 1.0f);
		p->colLocal = Collision::CreateCapsule(glm::vec3(0, 0, 0), glm::vec3(0, 4, 0), 0.4f);
		trees.Add(p);
	}
}

/**
*	石壁の作成処理
*/
void MainGameScene::CreateStoneWall(glm::vec3 start){

	const Mesh::FilePtr meshStoneWall = meshBuffer.GetFile("Res/wall_stone.gltf");
	glm::vec3 position = start + glm::vec3(-3, 0, -3);
	position.y = heightMap.Height(position);
	StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
		meshStoneWall, "StoneWall", 100, position, glm::vec3(0, 0.5f, 0));
	p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
		glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(2, 2, 0.5f));
	objects.Add(p);
}
