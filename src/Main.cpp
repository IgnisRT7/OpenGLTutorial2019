/**
*	@file main.cpp
*/

#include "TitleScene.h"

//エントリーポイント
int main() {

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	for (;;) {

		const float deltaTime = 1.0f / 60.0f;

		sceneStack.Update(deltaTime);
		sceneStack.Render();
	}

	return 0;
}