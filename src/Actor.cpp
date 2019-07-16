/**
*	@file Actor.cpp
*/
#include "Actor.h"
#include <glm/gtc/matrix_transform.hpp>

/**
*	コンストラクタ
*
*	@param name		アクターの名前
*	@param health	耐久力
*	@param position	位置
*	@param rotation	回転
*	@param scale	拡大率
*
*	指定された名前、耐久力位置、回転拡大率によってアクターを初期化する
*/
Actor::Actor(const std::string& name, int health, const glm::vec3& position,
	const glm::vec3& rotation, const glm::vec3& scale):
	name(name),health(health),position(position),rotation(rotation),scale(scale){
}

/**
*	アクターの状態を更新する
*
*	@param deltaTime	経過時間
*
*	UpdateDrawData()より前に実行すること
*/
void Actor::Update(float deltaTime) {

	position += velocity * deltaTime;

	//衝突判定の更新
	//colWorld = colLocal;
	//colWorld.center += position;
	const glm::mat4 matT = glm::translate(glm::mat4(1), position);
	const glm::mat4 matR_Y = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0));
	const glm::mat4 matR_ZY = glm::rotate(matR_Y, rotation.z, glm::vec3(0, 0, -1));
	const glm::mat4 matR_XZY = glm::rotate(matR_ZY, rotation.x, glm::vec3(1, 0, 0));
	const glm::mat4 matS = glm::scale(glm::mat4(1), scale);
	const glm::mat4 matModel = matT * matR_XZY*matS;
	colWorld.type = colLocal.type;

	switch (colLocal.type){
	case Collision::Shape::Type::sphere:
		colWorld.s.center = matModel * glm::vec4(colLocal.s.center, 1);
		colWorld.s.r = colLocal.s.r;
		break;

	case Collision::Shape::Type::capsule:
		colWorld.c.seg.a = matModel * glm::vec4(colLocal.c.seg.a, 1);
		colWorld.c.seg.b = matModel * glm::vec4(colLocal.c.seg.b, 1);
		colWorld.c.r = colLocal.c.r;
		break;

	default:
		break;
	}

}

/**
*	描画情報の更新
*
*	@param deltaTime	経過時間
*
*	Update()より後に実行すること
*/
void Actor::UpdateDrawData(float deltaTime) {

}

/**
*	アクターの描画
*/
void Actor::Draw() {
}

/**
*	コンストラクタ
*
*	@param m		表示するメッシュ
*	@param name		アクターの名前
*	@param health	耐久力
*	@param position	位置
*	@param rotation	回転
*	@param scale	拡大率
*
*	指定されたメッシュ、名前、耐久力、位置、回転拡大率によってアクターを初期化する
*/
StaticMeshActor::StaticMeshActor(const Mesh::FilePtr& m, const std::string& name, int hp,
	const glm::vec3& position,const glm::vec3& rotation,const glm::vec3& scale):
Actor(name,hp,position,rotation,scale),mesh(m){
}

/**
*	描画
*/
void StaticMeshActor::Draw() {

	if (mesh) {

		const glm::mat4 matT = glm::translate(glm::mat4(1), position);
		const glm::mat4 matR_Y = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0));
		const glm::mat4 matR_ZY = glm::rotate(matR_Y, rotation.z, glm::vec3(0, 0, -1));
		const glm::mat4 matR_XZY = glm::rotate(matR_ZY, rotation.x, glm::vec3(1, 0, 0));
		const glm::mat4 matS = glm::scale(glm::mat4(1), scale);
		const glm::mat4 matModel = matT * matR_XZY*matS;
		Mesh::Draw(mesh, matModel);
	}
}

/**
*	格納可能なアクター数を確保する
*
*	@param reserveCount		アクター配列の確保数
*/
void ActorList::Reserve(size_t reserveCount) {

	actors.reserve(reserveCount);
}

/**
*	アクターを追加する
*
*	@param actor	追加するアクター
*/
void ActorList::Add(const ActorPtr& actor) {

	actors.push_back(actor);
}

/**
*	アクターを削除する
*
*	@param actor	削除するアクター
*/
bool ActorList::Remove(const ActorPtr& actor) {

	for (auto itr = actors.begin(); itr != actors.end(); ++itr) {
		if (*itr = actor) {
			actors.erase(itr);
			return true;
		}
	}
	return false;
}

/**
*	アクターの状態を更新する
*
*	@param deltaTime	前回の更新からの経過時間
*/
void ActorList::Update(float deltaTime) {

	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->Update(deltaTime);
		}
	}
}

/**
*	アクターの描画データを更新する
*
*	@param deltaTime	前回の更新からの経過時間
*/
void ActorList::UpdateDrawData(float deltaTime) {

	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->UpdateDrawData(deltaTime);
		}
	}
}

/**
*	Actorを描画する
*/
void ActorList::Draw() {

	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->Draw();
		}
	}
}

/**
*	衝突判定を行う
*
*	@param a		判定対象のアクターその1
*	@param b		判定対象のアクターその2
*	@param handler	衝突した場合に実行される関数
*/
void DetectCollision(const ActorPtr& a, const ActorPtr& b, CollisionhandlerType handler) {

	if (a->health <= 0 || b->health <= 0) {
		return;
	}

//	if (Collision::TestSphereSphere(a->colWorld, b->colWorld)) {
//		handler(a, b, b->colWorld.center);
//	}
	glm::vec3 pa, pb;
	if (Collision::TestShapeShape(a->colWorld, b->colWorld, &pa, &pb)) {
		handler(a, b, pb);
	}
}

/**
*	衝突判定を行う
*
*	@param a		判定対象のアクター
*	@param b		判定対象のアクターリスト
*	@param handler	衝突した場合に実行される関数
*/
void DetectCollision(const ActorPtr& a, ActorList& b, CollisionhandlerType handler) {

	if (a->health < 0) {
		return;
	}
	for (const ActorPtr& actorB : b) {
		if (actorB->health <= 0) {
			continue;
		}

		//if (Collision::TestSphereSphere(a->colWorld, actorB->colWorld)) {
		//	handler(a, actorB, actorB->colWorld.center);
		glm::vec3 pa, pb;
		if (Collision::TestShapeShape(a->colWorld, actorB->colWorld, &pa, &pb)) {
			handler(a, actorB, pb);
			if(a->health <= 0){
				break;
			}
		}
	}
}

/**
*	衝突判定を行う
*
*	@param a		判定対象のアクターリストその1
*	@param b		判定対象のアクターリストその2
*	@param handler	衝突した場合に実行される関数
*/
void DetectCollision(ActorList& a, ActorList& b, CollisionhandlerType handler) {

	for (const ActorPtr& actorA : a) {
		if (actorA->health <= 0) {
			continue;
		}
		for (const ActorPtr& actorB : b) {
			if (actorB->health <= 0) {
				continue;
			}
			//if (Collision::TestSphereSphere(actorA->colWorld, actorB->colWorld)) {
			//	handler(actorA, actorB, actorB->colWorld.center);
			glm::vec3 pa, pb;
			if (Collision::TestShapeShape(actorA->colWorld, actorB->colWorld, &pa, &pb)) {
				handler(actorA, actorB, pb);
				if (actorA->health <= 0) {
					break;
				}
			}
		}		
	}
}