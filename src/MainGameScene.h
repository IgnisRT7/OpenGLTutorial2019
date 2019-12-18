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
#include "FramebufferObject.h"
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

	FramebufferObjectPtr fboMain;			///< ���C���̃t���[���o�b�t�@�I�u�W�F�N�g

	Audio::SoundPtr bgm;				///< BGM�f�[�^

	std::mt19937 randGen;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);

		//��ʃp�����[�^
		float width = 1280;	///< ��ʂ̕�(�s�N�Z����)
		float height = 720;	///< ��ʂ̍���(�s�N�Z����)
		float near = 1;		///< �ŏ�z�l(���[�g��)
		float far = 500;	///< �ő�z�l(���[�g��)

		//�J�����p�����[�^
		float fNumber = 1.4f;				///< �G�t�E�i���o�[ = �J������F�l
		float fov = glm::radians(60.0f);	///< �t�B�[���h�E�I�u�E�r���[ = �J�����̎���p(���W�A��)
		float sensorSize = 36.0f;			///< �Z���T�[�E�T�C�Y=�J�����̃Z���T�[�̉���(�~��)

		//Update�֐��Ōv�Z����p�����[�^
		float focallength = 50.0f;		///< �t�H�[�J���E�����O�X = �œ_����(�~��)
		float aperture = 20.0f;			///< �A�p�[�`�� = �J��(�~��)
		float focalPlane = 10000.0f;	///< �t�H�[�J���E�v���[�� = �s���g�̍�������

		void Update(const glm::mat4& matView);
	};
	Camera camera;
};

#endif //MAINGAMESCENE_H_INCLUDE