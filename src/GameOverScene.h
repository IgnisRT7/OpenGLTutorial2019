/**
*	@file StatusScene.h
*/
#ifndef GAMEOVERSCENE_H_INCLUDE
#define GAMEOVERSCENE_H_INCLUDE
#include "Scene.h"
#include "Audio.h"
#include "GLFWEW.h"

/**
*	�^�C�g�����
*/
class GameOverScene : public Scene {
public:

	GameOverScene() :Scene("GameOverScene") {}
	virtual ~GameOverScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override {}
	virtual void Render() override {}
	virtual void Finalize() override;

private:

	Audio::SoundPtr bgm;
};


#endif	//GAMEOVERSCENE_H_INCLUDE