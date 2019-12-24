/**
*	@file MainGameScene.cpp
*/

#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"

#include "SkeletalMeshActor.h"
#include "TerrainGenerator.h"
#include "JizoActor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>


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

	randGen.seed(0);

	//メッシュバッファの初期化処理
	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));
	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadMesh("Res/TestTree.gltf");
	meshBuffer.LoadMesh("Res/wall_stone.gltf");
	meshBuffer.LoadMesh("Res/jizo_statue.gltf");
	meshBuffer.LoadSkeletalmesh("Res/oni_small.gltf");
	meshBuffer.LoadSkeletalmesh("Res/bikuni.gltf");
	meshBuffer.LoadSkeletalmesh("Res/ue4modeltest.gltf");

	lightBuffer.Init(1);
	lightBuffer.BindToShader(meshBuffer.GetStaticMeshShader());
	lightBuffer.BindToShader(meshBuffer.GetTerrainShader());
	lightBuffer.BindToShader(meshBuffer.GetWaterShader());

	//FBOを作成する
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	fboMain = FramebufferObject::Create(window.Width(), window.Height(), GL_RGBA16F);
	Mesh::FilePtr rt = meshBuffer.AddPlane("RenderTarget");
	if (rt) {
		rt->materials[0].program = Shader::Program::Create("Res/DepthOfField.vert", "Res/DepthOfField.frag");
		rt->materials[0].texture[0] = fboMain->GetColorTexture();
		rt->materials[0].texture[1] = fboMain->GetDepthTexture();
	}
	if (!rt || !rt->materials[0].program) {
		return false;
	}

	//DoF描画用のFBOを作る
	fboDepthOfField = FramebufferObject::Create(window.Width(), window.Height(), GL_RGBA16F);

	//元解像度の縦横1/2(面積が1/4)の大きさのブルーム用FBOを作る
	int w = window.Width();
	int h = window.Height();
	for (int j = 0; j < sizeof(fboBloom) / sizeof(fboBloom[0]); ++j) {

		w /= 2;
		h /= 2;
		for (int i = 0; i < sizeof(fboBloom[0]) / sizeof(fboBloom[0][0]); ++i) {
			fboBloom[j][i] = FramebufferObject::Create(w, h, GL_RGBA16F, FrameBufferType::ColorOnly);
			if (!fboBloom[j][i]) {
				return false;
			}
		}
	}

	//ブルーム・エフェクト用の平面ポリゴンメッシュを作成する
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("BrightPassFilter")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/BrightPassFilter.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("NormalBlur")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/NormalBlur.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("Simple")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/Simple.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (glGetError()) {
		std::cout << "[error]: " << __func__ << ": ブルーム用メッシュの作成に失敗\n";
		return false;
	}

	//地形ジェネレータの初期化
	TerrainGenerator::Controller controller;
	controller.Generate();

	//ハイトマップを作成する

	bool isAutoGenerate = false;

	if (isAutoGenerate) {
		if (!heightMap.LoadFromTextureImage(controller.ImageData(), 10, 0)) {
			return false;
		}
	}
	else {
		if (!heightMap.LoadFromFile("Res/Terrain.tga", 50.0f, 0.5f)) {
			return false;
		}
	}

	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}
	if (!heightMap.CreateWaterMesh(meshBuffer, "Water", -15)) {
		return false;
	}


	//プレイヤーの作成処理
	glm::vec3 startPos(100, 0, 100);
	startPos.y= heightMap.Height(startPos);
	player = std::make_shared<PlayerActor>(&heightMap, meshBuffer, startPos);
	player->colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);

	//ディレクショナルライトを配置
	lights.Add(std::make_shared<DirectionalLightActor>("DirectionalLight", glm::vec3(0.8), glm::normalize(glm::vec3(1, -2, -1))));

	//ポイントライトを配置
	for (int i = 0; i < 50; ++i) {

		std::string pointLightName = "PointLight";
		pointLightName += std::to_string(i);
		glm::vec3 color(10, 0.8f, 0.5f);
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(80, 120)(randGen));
		position.z = static_cast<float>(std::uniform_int_distribution<>(80, 120)(randGen));
		position.y = heightMap.Height(position) + 1;
		lights.Add(std::make_shared<PointLightActor>("PointLight", color, position));

	}

	lights.Update(0);
	lightBuffer.Update(lights, glm::vec3(0.1f, 0.05f, 0.15f));
	heightMap.UpdateLightIndex(lights);

	//お地蔵様を配置
	for (int i = 0; i < 4; ++i) {
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(50, 150)(randGen));
		position.z = static_cast<float>(std::uniform_int_distribution<>(50, 150)(randGen));
		position.y = heightMap.Height(position);

		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0.0f, 3.14f * 2.0f)(randGen);
		JizoActorPtr p = std::make_shared<JizoActor>(meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);

		p->scale = glm::vec3(3);//見つけやすいように拡大
		objects.Add(p);
	}

#if 0
	SpawnKooni(1);
#endif

	SpawnTree(100);
	CreateStoneWall(startPos);

	bgm = Audio::Engine::Instance().Prepare("Res/Audio/mainScene.wav");
	bgm->Play(Audio::Flag_Loop);




	return true;
}

/**
*	プレイヤーの入力を処理する
*/
void MainGameScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = window.GetGamePad();

	player->ProcessInput();

	if (gamepad.buttonDown & GamePad::R) {
		Audio::Engine::Instance().Prepare("Res/Audio/select.wav")->Play();
		SceneStack::Instance().Push(std::make_shared<StatusScene>());
	}
	else if (gamepad.buttonDown&GamePad::L) {
		SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
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
		camera.position = camera.target + glm::vec3(0, 8, 13);
	}

	player->Update(deltaTime);
	enemies.Update(deltaTime);
	trees.Update(deltaTime);
	objects.Update(deltaTime);
	lights.Update(deltaTime);

	DetectCollision(player, enemies);
	DetectCollision(player, trees);
	DetectCollision(player, objects);

	//プレイヤーの攻撃判定
	ActorPtr attackCollision = player->GetAttackCollision();
	if (attackCollision) {
		bool hit = false;
		DetectCollision(attackCollision, enemies, [this, &hit](const ActorPtr& a, const ActorPtr& b, const glm::vec3&p) {
			SkeletalMeshActorPtr bb = std::static_pointer_cast<SkeletalMeshActor>(b);
			bb->health -= a->health;
			if (bb->health <= 0) {
				bb->colLocal = Collision::Shape{};
				bb->health = 1;
				bb->GetMesh()->Play("Down", false);
			}
			else {
				bb->GetMesh()->Play("Hit", false);
			}
			hit = true;
		});

		if (hit) {
			attackCollision->health = 0;
		}
	}

	//死亡アニメーションの終わった敵を消す
	for (auto& e : enemies) {
		SkeletalMeshActorPtr enemy = std::static_pointer_cast<SkeletalMeshActor>(e);
		Mesh::SkeletalMeshPtr mesh = enemy->GetMesh();
		if (mesh->IsFinished()) {
			if (mesh->GetAnimation() == "Down") {
				enemy->health = 0;
			}
			else {
				mesh->Play("Wait");
			}
		}
	}

	//ライトの更新
	glm::vec3 ambientColor(0.1f, 0.05f, 0.15f);
	lightBuffer.Update(lights, ambientColor);
	for (auto e : trees) {
		const std::vector<ActorPtr> neighborhood = lights.FindNearbyActors(e->position, 20);
		std::vector<int> pointLightIndex;
		std::vector<int> spotLightIndex;
		pointLightIndex.reserve(neighborhood.size());
		spotLightIndex.reserve(neighborhood.size());
		for (auto light : neighborhood) {

			if (PointLightActorPtr p = std::dynamic_pointer_cast<PointLightActor>(light)) {
				if (pointLightIndex.size() < 8) {
					pointLightIndex.push_back(p->index);
				}
			}
			else if (SpotLightActorPtr p = std::dynamic_pointer_cast<SpotLightActor>(light)) {
				if (spotLightIndex.size() < 8) {
					spotLightIndex.push_back(p->index);
				}
			}
		}

		StaticMeshActorPtr p = std::static_pointer_cast<StaticMeshActor>(e);
		p->SetPointLightList(pointLightIndex);
		p->SetSpotLightList(spotLightIndex);
	}


	//敵を全滅させたら目的達成フラグをtrueにする
	if (jizoId >= 0) {
		if (enemies.Empty()) {
			achivements[jizoId] = true;
			jizoId = -1;
		}
	}

	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);
	trees.UpdateDrawData(deltaTime);
	objects.UpdateDrawData(deltaTime);

	const float w = static_cast<float>(window.Width());
	const float h = static_cast<float>(window.Height());
	const float lineHeight = fontRenderer.LineHeight();
	//TODO: エラー発生中
//	fontRenderer.BeginUpdate();
//	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h*0.5f - lineHeight), L"メインゲーム画面");
//	fontRenderer.EndUpdate();
}

/**
*	描画処理
*/
void MainGameScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);

	glEnable(GL_DEPTH);

	lightBuffer.Upload();
	lightBuffer.Bind();

	// FBOに描画
	glBindFramebuffer(GL_FRAMEBUFFER, fboMain->GetFramebuffer());
	const auto texMain = fboMain->GetColorTexture();
	glViewport(0, 0, texMain->Width(), texMain->Height());
	glClearColor(0.5f, 0.6f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	const glm::mat4 matView = glm::lookAt(camera.position, camera.target, camera.up);

	const float aspectRatio = static_cast<float>(window.Width() / static_cast<float>(window.Height()));
	const glm::mat4 matProj = glm::perspective(camera.fov, aspectRatio, camera.near, camera.far);

	meshBuffer.SetViewProjectionMatrix(matProj * matView);
	meshBuffer.SetCameraPosition(camera.position);
	meshBuffer.SetTime(window.Time());

	//キューブの行列変換
	glm::vec3 cubePos(120, 0, 120);
	cubePos.y = heightMap.Height(cubePos);
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);
	Mesh::Draw(meshBuffer.GetFile("Cube"), matModel);

	//地形の行列変換
	const Mesh::FilePtr& terrainFile = meshBuffer.GetFile("Terrain");
	terrainFile->materials[0].program->Use();
	terrainFile->materials[0].program->SetViewProjectionMatrix(matProj * matView);
	Mesh::Draw(terrainFile, glm::mat4(1));

	//木の行列変換
	glm::vec3 treePos(110, 0, 110);
	treePos.y = heightMap.Height(treePos);
	const glm::mat4 matTreeModel = glm::translate(glm::mat4(1), treePos) * glm::scale(glm::mat4(1), glm::vec3(3));
	Mesh::Draw(meshBuffer.GetFile("Res/red_pine_tree.gltf"), matTreeModel);

	enemies.Draw();
	trees.Draw();
	objects.Draw();
	player->Draw();

	//水の描画処理
	Mesh::Draw(meshBuffer.GetFile("Water"), glm::mat4(1));

	//被写界深度エフェクト
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboDepthOfField->GetFramebuffer());
		const auto tex = fboDepthOfField->GetColorTexture();
		glViewport(0, 0, tex->Width(), tex->Height());

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		camera.Update(matView);

		Mesh::FilePtr mesh = meshBuffer.GetFile("RenderTarget");

		Shader::ProgramPtr prog = mesh->materials[0].program;
		prog->Use();
		prog->SetViewInfo(static_cast<float>(window.Width()), static_cast<float>(window.Height()),
			camera.near, camera.far);

		Mesh::Draw(mesh, glm::mat4(1));

		//fontRenderer.Draw(screenSize);
	}

	//ブルーム・エフェクト
	{
		auto tex = fboBloom[0][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[0][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_COLOR_BUFFER_BIT);
		Mesh::FilePtr mesh = meshBuffer.GetFile("BrightPassFilter");
		mesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
		Mesh::Draw(mesh, glm::mat4(1));
	}

	//縮小コピー
	Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
	for (int i = 0; i < sizeof(fboBloom) / sizeof(fboBloom[0])- 1; ++i) {
		auto tex = fboBloom[i + 1][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i + 1][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_COLOR_BUFFER_BIT);
		simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));
	}

	//ガウスぼかし
	Mesh::FilePtr blurMesh = meshBuffer.GetFile("NormalBlur");
	Shader::ProgramPtr progBlur = blurMesh->materials[0].program;
	for (int i = sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; i >= 0; --i) {
		auto tex = fboBloom[i][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i][1]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_COLOR_BUFFER_BIT);
		progBlur->Use();
		progBlur->SetBlurDirection(1.0f / static_cast<float>(tex->Width()), 0.0f);
		blurMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
		Mesh::Draw(blurMesh, glm::mat4(1));

		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i][0]->GetFramebuffer());
		glClear(GL_COLOR_BUFFER_BIT);
		progBlur->Use();
		progBlur->SetBlurDirection(0.0f, 1.0f / static_cast<float>(tex->Height()));
		blurMesh->materials[0].texture[0] = fboBloom[i][1]->GetColorTexture();
		Mesh::Draw(blurMesh, glm::mat4(1));
	}

	//拡大&加算合成
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (int i = sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; i > 0; --i) {
		auto tex = fboBloom[i - 1][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i - 1][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));
	}

	//すべてをデフォルト・フレームバッファに合成描画
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.Width(), window.Height());

		const glm::vec2 screenSize(window.Height());
		spriteRenderer.Draw(screenSize);

		//被写界深度エフェクト適用後の画像を描画
		glDisable(GL_BLEND);
		Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
		simpleMesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));

		//拡散光を描画
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		simpleMesh->materials[0].texture[0] = fboBloom[0][0]->GetColorTexture();
		Mesh::Draw(simpleMesh,glm::mat4(1));
	}

}

/**
*	終了処理
*/
void MainGameScene::Finalize(){

	bgm->Stop();
}

/**
*	お地蔵さまに触れたときの処理
*
*	@param id	お地蔵様の番号
*	@param pos	お地蔵様の座標
*
*	@retval true	処理成功
*	@retval false	既に戦闘中なので処理しなかった
*/
bool MainGameScene::HandleJizoEffects(int id, const glm::vec3& pos){

	if (jizoId >= 0) {
		return false;
	}
	jizoId = id;
	const size_t oniCount = 8;
	for (size_t i = 0; i < oniCount; i++) {

		glm::vec3 position(pos);
		position.x += std::uniform_real_distribution<float>(-15, 15)(randGen);
		position.z += std::uniform_real_distribution<float>(-15, 15)(randGen);
		position.y = heightMap.Height(position);

		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 3.14f * 2.0f)(randGen);
		const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
		SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(mesh, "Kooni", 13, position, rotation);
		p->GetMesh()->Play("Run");
		p->colLocal = Collision::CreateSphere(glm::vec3(0), 1.0f);
		enemies.Add(p);
	}

	return true;
}

/**
*	カメラのparameterを更新する
*
*	@param matView 更新に使用するビュー行列
*/
void MainGameScene::Camera::Update(const glm::mat4& matView) {

	const glm::vec4 pos = matView * glm::vec4(target, 1);
	focalPlane = pos.z * -1000.0f;

	const float imageDistance = sensorSize * 0.5f / glm::tan(fov * 0.5f);
	focallength = 1.0f / ((1.0f / focalPlane) + (1.0f / imageDistance));

	aperture = focallength / fNumber;
}


/**
*	再生処理
*/
void MainGameScene::Play(){

}

/**
*	停止処理
*/
void MainGameScene::Stop(){
}

/**
*	小鬼のスポーン処理
*
*	@param n	スポーン数
*/
void MainGameScene::SpawnKooni(int n) {

	//敵を配置
	const size_t oniCount = n;
	enemies.Reserve(oniCount);
	for (size_t i = 0; i < oniCount; ++i) {
		//敵の位置を(50,50)-(150,150)の範囲からランダムに選択
		glm::vec3 position(0);
		position.x = std::uniform_real_distribution<float>(50, 150)(randGen);
		position.z = std::uniform_real_distribution<float>(50, 150)(randGen);
		position.y = heightMap.Height(position);

		//敵の向きをランダムに選択
		glm::vec3 rotation(0);
		std::string enemyName = true ? "SK_Mannequin_LOD0.001" : "oni_small";
		std::string animName = true ? "walk" : "Attack";
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(randGen);
		const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh(enemyName.c_str());
		SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(mesh, "Kooni", 13, position, rotation);
		p->GetMesh()->Play(animName.c_str());

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

	//木を配置
	const size_t treeCount = n;
	trees.Reserve(treeCount);
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/red_pine_tree.gltf");
	//for (size_t i = 0; i < treeCount; ++i) {
	for(size_t i=0;i<lights.Size();++i){

		//木の位置を(50,50)-(150,150)の範囲からランダムに選択
		glm::vec3 position(0);
	//	position.x = std::uniform_real_distribution<float>(50, 150)(randGen);
	//	position.z = std::uniform_real_distribution<float>(50, 150)(randGen);
	//	position.y = heightMap.Height(position);
		//const ActorPtr& lightActor = lights.Get(i);
		auto lightActor = lights.Get(i);
		if (!lightActor.expired()) {
			position = lightActor.lock()->position;
		}


		//木の向きをランダムに選択
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(randGen);
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

