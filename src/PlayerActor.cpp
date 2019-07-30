/**
*	@file PlayerActor.cpp
*/
#include "PlayerActor.h"
#include "SkeletalMesh.h"
#include <glm/gtc/matrix_transform.hpp>

/**
*	コンストラクタ
*
*	@param hm		プレイヤーの着地判定に使用する高さマップ
*	@param buffer	プレイヤーのメッシュデータを持つメッシュバッファ
*	@param pos		プレイヤーの初期座標
*	@param rot		プレイヤー初期方向
*/
PlayerActor::PlayerActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot) :
	SkeletalMeshActor(buffer.GetSkeletalMesh("Bikuni"), "Player", 13, pos, rot), heightMap(hm) {

	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);

	GetMesh()->Play("Idle");
	state = State::idle;
}

/**
*	更新処理
*
*	@param deltaTime	経過時間
*/
void PlayerActor::Update(float deltaTime) {

	//座標の更新
	SkeletalMeshActor::Update(deltaTime);

	//接地判定
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y < groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
		isInAir = false;
	}
	else if (position.y > groundHeight) {
		//乗っている物体から離れたら空中判定にする
		if (boardingActor) {
			Collision::Shape col = colWorld;
			col.c.r += 0.1f;	//衝突判定を少し大きくする
			glm::vec3 pa, pb;
			if (!Collision::TestShapeShape(col, boardingActor->colWorld, &pa, &pb)) {
				boardingActor.reset();
			}
		}
	}

	//落下判定
	const bool isFloating = position.y > groundHeight + 0.1f;	// 地面から浮いているか
	if (!isInAir && isFloating && !boardingActor) {
		isInAir = true;
	}

	//重力を加える
	if (isInAir) {
		velocity.y -= gravity * deltaTime;
	}

	//アニメーションの更新
	switch (state) {
	case State::run:
		if (isInAir) {
			GetMesh()->Play("Jump");
			state = State::jump;
		}
		else {
			const float horizontalSpeed = velocity.x * velocity.x + velocity.z * velocity.z;
			if (horizontalSpeed == 0) {
				GetMesh()->Play("Idle");
				state = State::idle;
			}
		}
		break;

	case State::idle:
		if (isInAir) {
			GetMesh()->Play("Jump");
			state = State::jump;
		}
		else {
			const float horizontalSpeed = velocity.x * velocity.x * velocity.z * velocity.z;
			if (horizontalSpeed != 0) {
				GetMesh()->Play("Run");
				state = State::run;
			}
		}
		break;

	case State::jump:
		if (!isInAir) {
			GetMesh()->Play("Idle");
			state = State::idle;
		}
		break;
	}

}

/**
*	入力を処理する
*/
void PlayerActor::ProcessInput() {

	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
	CheckRun(gamepad);
	CheckJump(gamepad);
}

/**
*	移動操作を処理する
*
*	@param gamepad	ゲームパッド入力
*/
void PlayerActor::CheckRun(const GamePad& gamepad) {

	//空中にいるときは移動できない
	if (isInAir) {
		return;
	}

	//方向キーから移動方向を計算
	const glm::vec3 front(0, 0, -1);
	const glm::vec3 left(-1, 0, 0);
	glm::vec3 move(0);

	if (gamepad.buttons & GamePad::DPAD_UP) {
		move += front;
	}
	else if (gamepad.buttons& GamePad::DPAD_DOWN) {
		move -= front;
	}
	if (gamepad.buttons & GamePad::DPAD_LEFT) {
		move += left;
	}
	else if (gamepad.buttons & GamePad::DPAD_RIGHT) {
		move -= left;
	}

	//移動が行われていたら、移動方向に応じて向きと速度を更新
	if (glm::dot(move, move)) {
		//向きを更新
		move = glm::normalize(move);
		rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);

		//物体に載っていない時は地形の勾配を考慮して移動方向を調整する
		if (!boardingActor) {
			//移動方向の地形の勾配(gradient)を計算
			const float minGradient = glm::radians(-60.0f);	//沿うことのできる勾配の最小値
			const float maxGradient = glm::radians(60.0f);	//沿うことのできる勾配の最大値
			const float frontY = heightMap->Height(position + move * 0.05f) - position.y - 0.01f;
			const float gradient = glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//地形に沿うように移動速度を設定
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			move = glm::rotate(glm::mat4(1), gradient, axis)*glm::vec4(move, 1.0f);
		}
		velocity = move * moveSpeed;
	}
	else {
		//移動していないので速度を0にする
		velocity = glm::vec3(0);
	}
}

/**
*	ジャンプ操作を処理する
*
*	@param gamepad ゲームパッド入力
*/
void PlayerActor::CheckJump(const GamePad& gamepad) {

	if (isInAir) {
		return;
	}
	if (gamepad.buttonDown&GamePad::B) {
		Jump();
	}
}

/**
*	ジャンプさせる
*/
void PlayerActor::Jump() {
	velocity.y = 5.0f;
	boardingActor.reset();
	isInAir = true;
}

/**
*	プレイヤーが乗っている物体を設定する
*
*	@param p	乗っている物体
*/
void PlayerActor::SetBoardingActor(ActorPtr p) {

	boardingActor = p;
	if (p) {
		isInAir = false;
	}
}

/**
*	衝突ハンドラ
*/
void PlayerActor::OnHit(const ActorPtr& b, const glm::vec3& p) {

	const glm::vec3 v = colWorld.s.center - p;
	//衝突位置との距離が近すぎないか調べる
	if (dot(v, v) > FLT_EPSILON) {
		//aをbに重ならない位置まで移動
		const glm::vec3 vn = normalize(v);
		//const float radiusSum = a->colWorld.r + b->colWorld.r;
		float radiusSum = colWorld.s.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;
		}

		const float distance = radiusSum - glm::length(v) + 0.01f;
		position += vn * distance;

		colWorld.s.center += vn * distance;
		if (velocity.y < 0 && vn.y >= glm::cos(glm::radians(60.0f))) {
			velocity.y = 0;
		}
	}
	else {
		//移動を取り消す(距離が近すぎる場合の例外処理)
		const float dletaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = velocity * static_cast<float>(GLFWEW::Window::Instance().DeltaTime());;
		colWorld.s.center -= deltaVelocity;
	}
	SetBoardingActor(b);
}