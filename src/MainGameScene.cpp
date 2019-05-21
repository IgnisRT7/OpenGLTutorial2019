/**
*	@file MainGameScene.cpp
*/

#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"

/**
*	ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ğˆ—‚·‚é
*/
void MainGameScene::ProcessInput() {

	if (!frag) {
		frag = true;
		SceneStack::Instance().Push(std::make_shared<StatusScene>());
	}
	else {

		SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
	}
}