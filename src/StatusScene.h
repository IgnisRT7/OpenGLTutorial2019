/**
*	@file StatusScene.h
*/
#ifndef STATUSSCENE_H_INCLUDE
#define STATUSSCENE_H_INCLUDE
#include "Scene.h"
#include "Audio.h"
#include "GLFWEW.h"

/**
*	ƒ^ƒCƒgƒ‹‰æ–Ê
*/
class StatusScene : public Scene {
public:

	StatusScene() :Scene("StatusScene") {}
	virtual ~StatusScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override {}
	virtual void Render() override {}
	virtual void Finalize() override {}

private:

	//Audio::SoundPtr bgm;
};


#endif	//STATUSSCENE_H_INCLUDE