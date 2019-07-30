/**
*	@file PlayerActor.cpp
*/
#pragma once

#ifndef PLAYERACTOR_H_INCLUDED
#define PLAYERACTOR_H_INCLUDED

#include "Actor.h"
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "Terrain.h"
#include <memory>

/**
*	プレイヤーアクタ
*/
class PlayerActor : public SkeletalMeshActor {
public:

	//PlayerActor(const Mesh::FilePtr& mesh, const glm::vec3& pos,
	//	const glm::vec3& rot, const Terrain::HeightMap* hm);
	PlayerActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0));
	virtual ~PlayerActor() = default;

	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&) override;

	void Jump();
	void ProcessInput();
	void SetBoardingActor(ActorPtr);

private:

	void CheckRun(const GamePad& gamepad);
	void CheckJump(const GamePad& gamepad);

	/// アニメーション状態
	enum class State {
		idle,	///< 停止
		run,	///< 移動
		jump,	///< ジャンプ
	};
	State state = State::idle;	///< 現在のアニメーション状態
	bool isInAir = false;		///< 空中判定フラグ
	ActorPtr boardingActor;		///< 載っているアクタ―
	float moveSpeed = 5.0f;		///< 移動速度

	const Terrain::HeightMap* heightMap = nullptr;
};
using PlayerActorPtr = std::shared_ptr<PlayerActor>;

#endif // PLAAYERACTOR_H_INCLUDED
