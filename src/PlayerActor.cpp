/**
*	@file PlayerActor.cpp
*/
#include "PlayerActor.h"

/**
*	コンストラクタ
*/
PlayerActor::PlayerActor(const Mesh::FilePtr& mesh,
	const glm::vec3& pos, const glm::vec3& rot, const Terrain::HeightMap* hm):
	StaticMeshActor(mesh,"Player",20,pos,rot),heightMap(hm){

	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);
}

/**
*	更新処理
*
*	@param deltaTime	経過時間
*/
void PlayerActor::Update(float deltaTime) {

	//座標の更新
	StaticMeshActor::Update(deltaTime);

	//接地判定
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y < groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
	}
	else if (position.y > groundHeight) {
		velocity.y -= gravity * deltaTime;
	}
}

/**
*	ジャンプさせる
*/
void PlayerActor::Jump() {
	velocity.y = 5.0f;
}