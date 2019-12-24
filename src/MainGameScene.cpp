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
*	����������
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

	//���b�V���o�b�t�@�̏���������
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

	//FBO���쐬����
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

	//DoF�`��p��FBO�����
	fboDepthOfField = FramebufferObject::Create(window.Width(), window.Height(), GL_RGBA16F);

	//���𑜓x�̏c��1/2(�ʐς�1/4)�̑傫���̃u���[���pFBO�����
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

	//�u���[���E�G�t�F�N�g�p�̕��ʃ|���S�����b�V�����쐬����
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
		std::cout << "[error]: " << __func__ << ": �u���[���p���b�V���̍쐬�Ɏ��s\n";
		return false;
	}

	//�n�`�W�F�l���[�^�̏�����
	TerrainGenerator::Controller controller;
	controller.Generate();

	//�n�C�g�}�b�v���쐬����

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


	//�v���C���[�̍쐬����
	glm::vec3 startPos(100, 0, 100);
	startPos.y= heightMap.Height(startPos);
	player = std::make_shared<PlayerActor>(&heightMap, meshBuffer, startPos);
	player->colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);

	//�f�B���N�V���i�����C�g��z�u
	lights.Add(std::make_shared<DirectionalLightActor>("DirectionalLight", glm::vec3(0.8), glm::normalize(glm::vec3(1, -2, -1))));

	//�|�C���g���C�g��z�u
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

	//���n���l��z�u
	for (int i = 0; i < 4; ++i) {
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(50, 150)(randGen));
		position.z = static_cast<float>(std::uniform_int_distribution<>(50, 150)(randGen));
		position.y = heightMap.Height(position);

		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0.0f, 3.14f * 2.0f)(randGen);
		JizoActorPtr p = std::make_shared<JizoActor>(meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);

		p->scale = glm::vec3(3);//�����₷���悤�Ɋg��
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
*	�v���C���[�̓��͂���������
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
*	�X�V����
*
*	@param deltaTime	�o�ߎ���
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

	//�v���C���[�̍U������
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

	//���S�A�j���[�V�����̏I������G������
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

	//���C�g�̍X�V
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


	//�G��S�ł�������ړI�B���t���O��true�ɂ���
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
	//TODO: �G���[������
//	fontRenderer.BeginUpdate();
//	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h*0.5f - lineHeight), L"���C���Q�[�����");
//	fontRenderer.EndUpdate();
}

/**
*	�`�揈��
*/
void MainGameScene::Render() {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);

	glEnable(GL_DEPTH);

	lightBuffer.Upload();
	lightBuffer.Bind();

	// FBO�ɕ`��
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

	//�L���[�u�̍s��ϊ�
	glm::vec3 cubePos(120, 0, 120);
	cubePos.y = heightMap.Height(cubePos);
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);
	Mesh::Draw(meshBuffer.GetFile("Cube"), matModel);

	//�n�`�̍s��ϊ�
	const Mesh::FilePtr& terrainFile = meshBuffer.GetFile("Terrain");
	terrainFile->materials[0].program->Use();
	terrainFile->materials[0].program->SetViewProjectionMatrix(matProj * matView);
	Mesh::Draw(terrainFile, glm::mat4(1));

	//�؂̍s��ϊ�
	glm::vec3 treePos(110, 0, 110);
	treePos.y = heightMap.Height(treePos);
	const glm::mat4 matTreeModel = glm::translate(glm::mat4(1), treePos) * glm::scale(glm::mat4(1), glm::vec3(3));
	Mesh::Draw(meshBuffer.GetFile("Res/red_pine_tree.gltf"), matTreeModel);

	enemies.Draw();
	trees.Draw();
	objects.Draw();
	player->Draw();

	//���̕`�揈��
	Mesh::Draw(meshBuffer.GetFile("Water"), glm::mat4(1));

	//��ʊE�[�x�G�t�F�N�g
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

	//�u���[���E�G�t�F�N�g
	{
		auto tex = fboBloom[0][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[0][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_COLOR_BUFFER_BIT);
		Mesh::FilePtr mesh = meshBuffer.GetFile("BrightPassFilter");
		mesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
		Mesh::Draw(mesh, glm::mat4(1));
	}

	//�k���R�s�[
	Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
	for (int i = 0; i < sizeof(fboBloom) / sizeof(fboBloom[0])- 1; ++i) {
		auto tex = fboBloom[i + 1][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i + 1][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_COLOR_BUFFER_BIT);
		simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));
	}

	//�K�E�X�ڂ���
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

	//�g��&���Z����
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (int i = sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; i > 0; --i) {
		auto tex = fboBloom[i - 1][0]->GetColorTexture();
		glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i - 1][0]->GetFramebuffer());
		glViewport(0, 0, tex->Width(), tex->Height());
		simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));
	}

	//���ׂĂ��f�t�H���g�E�t���[���o�b�t�@�ɍ����`��
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.Width(), window.Height());

		const glm::vec2 screenSize(window.Height());
		spriteRenderer.Draw(screenSize);

		//��ʊE�[�x�G�t�F�N�g�K�p��̉摜��`��
		glDisable(GL_BLEND);
		Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
		simpleMesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));

		//�g�U����`��
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		simpleMesh->materials[0].texture[0] = fboBloom[0][0]->GetColorTexture();
		Mesh::Draw(simpleMesh,glm::mat4(1));
	}

}

/**
*	�I������
*/
void MainGameScene::Finalize(){

	bgm->Stop();
}

/**
*	���n�����܂ɐG�ꂽ�Ƃ��̏���
*
*	@param id	���n���l�̔ԍ�
*	@param pos	���n���l�̍��W
*
*	@retval true	��������
*	@retval false	���ɐ퓬���Ȃ̂ŏ������Ȃ�����
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
*	�J������parameter���X�V����
*
*	@param matView �X�V�Ɏg�p����r���[�s��
*/
void MainGameScene::Camera::Update(const glm::mat4& matView) {

	const glm::vec4 pos = matView * glm::vec4(target, 1);
	focalPlane = pos.z * -1000.0f;

	const float imageDistance = sensorSize * 0.5f / glm::tan(fov * 0.5f);
	focallength = 1.0f / ((1.0f / focalPlane) + (1.0f / imageDistance));

	aperture = focallength / fNumber;
}


/**
*	�Đ�����
*/
void MainGameScene::Play(){

}

/**
*	��~����
*/
void MainGameScene::Stop(){
}

/**
*	���S�̃X�|�[������
*
*	@param n	�X�|�[����
*/
void MainGameScene::SpawnKooni(int n) {

	//�G��z�u
	const size_t oniCount = n;
	enemies.Reserve(oniCount);
	for (size_t i = 0; i < oniCount; ++i) {
		//�G�̈ʒu��(50,50)-(150,150)�͈̔͂��烉���_���ɑI��
		glm::vec3 position(0);
		position.x = std::uniform_real_distribution<float>(50, 150)(randGen);
		position.z = std::uniform_real_distribution<float>(50, 150)(randGen);
		position.y = heightMap.Height(position);

		//�G�̌����������_���ɑI��
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
*	�؂̃X�|�[������
*
*	@param n	�X�|�[����
*/
void MainGameScene::SpawnTree(int n) {

	//�؂�z�u
	const size_t treeCount = n;
	trees.Reserve(treeCount);
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/red_pine_tree.gltf");
	//for (size_t i = 0; i < treeCount; ++i) {
	for(size_t i=0;i<lights.Size();++i){

		//�؂̈ʒu��(50,50)-(150,150)�͈̔͂��烉���_���ɑI��
		glm::vec3 position(0);
	//	position.x = std::uniform_real_distribution<float>(50, 150)(randGen);
	//	position.z = std::uniform_real_distribution<float>(50, 150)(randGen);
	//	position.y = heightMap.Height(position);
		//const ActorPtr& lightActor = lights.Get(i);
		auto lightActor = lights.Get(i);
		if (!lightActor.expired()) {
			position = lightActor.lock()->position;
		}


		//�؂̌����������_���ɑI��
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(randGen);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(mesh, "Tree", 13, position, rotation);
		//p->colLocal = Collision::Sphere(glm::vec3(0), 1.0f);
		p->colLocal = Collision::CreateCapsule(glm::vec3(0, 0, 0), glm::vec3(0, 4, 0), 0.4f);
		trees.Add(p);
	}
}

/**
*	�Εǂ̍쐬����
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

