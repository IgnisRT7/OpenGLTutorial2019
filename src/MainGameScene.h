/**
*	@file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDE
#define MAINGAMESCENE_H_INCLUDE

#include "GLFWEW.h"
#include "Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include "Audio.h"
#include "Light.h"
#include <vector>
#include <random>

/**
*	���C���Q�[�����
*/
class MainGameScene : public Scene {
public:

	MainGameScene() : Scene("MainGameScene") {}
	virtual ~MainGameScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override;

	bool HandleJizoEffects(int id, const glm::vec3& pos);

	virtual void Play() override;
	virtual void Stop() override;

	void SpawnKooni(int n);
	void SpawnTree(int n);
	void CreateStoneWall(glm::vec3 s);

private:

	int jizoId = -1;					///< ���ݐ퓬���̂��n���l��ID
	bool achivements[4] = { false,false,false,false };	///< �G�������

	bool frag = false;
	SpriteRenderer spriteRenderer;		///< �X�v���C�g�`��Ǘ��N���X
	FontRenderer fontRenderer;			///< �t�H���g�`��Ǘ��N���X
	Mesh::Buffer meshBuffer;			///< ���b�V���o�b�t�@�Ǘ��N���X
	std::vector<Sprite> sprites;		///< �X�v���C�g�Q
	Terrain::HeightMap heightMap;		///< �n�`�n�C�g�f�[�^
	PlayerActorPtr player;				///< �v���C���[�f�[�^
	ActorList enemies;					///< �G���X�g�f�[�^
	ActorList trees;					///< �؃��X�g�f�[�^
	ActorList objects;					///< �I�u�W�F�N�g���X�g�f�[�^
	
	LightBuffer lightBuffer;
	ActorList lights;
	Audio::SoundPtr bgm;				///< BGM�f�[�^

	std::mt19937 randGen;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);
	};
	Camera camera;
};

#endif //MAINGAMESCENE_H_INCLUDE