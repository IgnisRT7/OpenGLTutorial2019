/**
*	@file Actor.h
*/
#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include <GL/glew.h>
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

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

public:

	std::string name;	///< アクターの名前
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 velocity = glm::vec3(0);	///< 速度
	int health = 0;	///< 体力
};
using ActorPtr = std::shared_ptr<Actor>;

/**
*	メッシュ表示機能付きのアクター
*/
class StaticMeshActor :public Actor {
public:

	StaticMeshActor(const Mesh::FilePtr& m, const std::string& name, int hp,
		const glm::vec3& position, const glm::vec3& rotation=glm::vec3(0),const glm::vec3& scale = glm::vec3(1));
	virtual ~StaticMeshActor() = default;
	virtual void Draw() override;

private:

	Mesh::FilePtr mesh;
};
using StaticMeshActorPtr = std::shared_ptr<StaticMeshActor>;


#endif
