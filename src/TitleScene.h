/**
*	@file TitleScene.h
*/
#ifndef TITLESCENE_H_INCLUDE
#define TITLESCENE_H_INCLUDE
#include "Scene.h"
#include <vector>
#include "Sprite.h"
#include "Font.h"
#include "Audio.h"

/**
*	タイトル画面
*/
class TitleScene : public Scene {
public:

	TitleScene() :Scene("TitleScene") {}
	virtual ~TitleScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

private:

	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	float timer = 0;

	Audio::SoundPtr bgm;
};


#endif	//TITLESCENE_H_INCLUDE