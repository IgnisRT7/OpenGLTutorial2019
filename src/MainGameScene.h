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
#include <vector>

/**
*	メインゲーム画面
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

private:

	bool frag = false;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Mesh::Buffer meshBuffer;
	std::vector<Sprite> sprites;
	Terrain::HeightMap heightMap;
};

#endif //MAINGAMESCENE_H_INCLUDE