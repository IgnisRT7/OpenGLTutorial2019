/**
*	@file StatusScene.h
*/
#ifndef GAMEOVERSCENE_H_INCLUDE
#define GAMEOVERSCENE_H_INCLUDE
#include "Scene.h"

/**
*	ƒ^ƒCƒgƒ‹‰æ–Ê
*/
class GameOverScene : public Scene {
public:

	GameOverScene() :Scene("GameOverScene") {}
	virtual ~GameOverScene() = default;

	virtual bool Initialize() override { return true; }
	virtual void ProcessInput() override;
	virtual void Update(float) override {}
	virtual void Render() override {}
	virtual void Finalize() override {}
};


#endif	//GAMEOVERSCENE_H_INCLUDE