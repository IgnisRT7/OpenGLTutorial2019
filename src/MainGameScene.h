/**
*	@file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDE
#define MAINGAMESCENE_H_INCLUDE
#include "Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include <vector>

/**
*	ÉÅÉCÉìÉQÅ[ÉÄâÊñ 
*/
class MainGameScene : public Scene {
public:

	MainGameScene() : Scene("MainGameScene") {}
	virtual ~MainGameScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	void SpawnKooni(int n);
	void SpawnTree(int n);
	void CreateStoneWall(glm::vec3 s);

private:

	bool frag = false;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Mesh::Buffer meshBuffer;
	std::vector<Sprite> sprites;
	Terrain::HeightMap heightMap;
	PlayerActorPtr player;
	ActorList enemies;
	ActorList trees;
	ActorList objects;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);
	};
	Camera camera;
};

#endif //MAINGAMESCENE_H_INCLUDE