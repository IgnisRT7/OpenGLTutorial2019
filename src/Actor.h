/**
*	@file Actor.h
*/
#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include <GL/glew.h>
#include "Mesh.h"
#include "Collision.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

class Actor;
using ActorPtr = std::shared_ptr<Actor>;

/**
*	シーンに配置するオブジェクト
*/
class Actor {
public:

	Actor(const std::string& name, int hp, const glm::vec3& pos,
		const glm::vec3& rot = glm::vec3(0), const glm::vec3& scale = glm::vec3(1));
	virtual ~Actor() = default;

	virtual void Update(float);
	virtual void UpdateDrawData(float);
	virtual void Draw();
	virtual void OnHit(const ActorPtr&, const glm::vec3& ){}

public:

	std::string name;	///< アクターの名前
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 velocity = glm::vec3(0);	///< 速度
	int health = 0;	///< 体力

//	Collision::Sphere colLocal;
//	Collision::Sphere colWorld;
	Collision::Shape colLocal;
	Collision::Shape colWorld;
};

/**
*	メッシュ表示機能付きのアクター
*/
class StaticMeshActor :public Actor {
public:

	StaticMeshActor(const Mesh::FilePtr& m, const std::string& name, int hp,
		const glm::vec3& position, const glm::vec3& rotation=glm::vec3(0),const glm::vec3& scale = glm::vec3(1));
	virtual ~StaticMeshActor() = default;
	virtual void Draw() override;

	void SetPointLightList(const std::vector<int>& v);
	void SetSpotLightList(const std::vector<int>& v);

private:

	Mesh::FilePtr mesh;

	int pointLightCount = 0;
	int pointLihgtIndex[8] = {};
	int spotLightCount = 0;
	int spotLightIndex[8] = {};
};
using StaticMeshActorPtr = std::shared_ptr<StaticMeshActor>;

/**
*	アクターをまとめて操作するクラス
*/
class ActorList {
public:

	//イテレーターを定義する
	using iterator = std::vector<ActorPtr>::iterator;
	using const_iterator = std::vector<ActorPtr>::const_iterator;

	ActorList() = default;
	~ActorList() = default;

	void Reserve(size_t);
	void Add(const ActorPtr&);
	bool Remove(const ActorPtr&);
	void Update(float);
	void UpdateDrawData(float);
	void Draw();
	bool Empty() const { return actors.empty(); }
	size_t Size()const { return actors.size(); }
	const std::weak_ptr<Actor> Get(int index) const {
		auto var = (actors.size() > index ? actors[index] : nullptr);
		return var;
	}

	//イテレーターを取得する関数
	iterator begin() { return actors.begin(); }
	iterator end() { return actors.end(); }
	const_iterator begin() const { return actors.begin(); }
	const_iterator end() const { return actors.end(); }

	std::vector<ActorPtr> FindNearbyActors(const glm::vec3& pos, float maxDistance) const;

private:

	std::vector<ActorPtr> actors;

	static const int mapGridSizeX = 10;
	static const int mapGridSizeY = 10;
	static const int sepalationSizeY = 20;
	static const int sepalationSizeX = 20;
	std::vector<ActorPtr> grid[sepalationSizeY][sepalationSizeX];
	glm::ivec2 CalcMapIndex(const glm::vec3& pos) const;
};

using CollisionhandlerType =
std::function<void(const ActorPtr&, const ActorPtr&, const glm::vec3&)>;
void DetectCollision(const ActorPtr& a, const ActorPtr& b, CollisionhandlerType handler = nullptr);
void DetectCollision(const ActorPtr& a, ActorList& b, CollisionhandlerType handler = nullptr);
void DetectCollision(ActorList& a, ActorList& b, CollisionhandlerType handler = nullptr);

#endif
