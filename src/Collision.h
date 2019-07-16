/**
*	@file Collision.h
*/
#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Collision {

	/**
	*	弾
	*/
	struct Sphere {
		Sphere(glm::vec3 p, float r) :center(p), r(r) {}
		Sphere() = default;
		glm::vec3 center = glm::vec3(0);///< 中心座標
		float r = 0; ///< 半径
	};

	/**
	*	線分
	*/
	struct Segment {
		glm::vec3 a = glm::vec3(0);	///< 線分の始点
		glm::vec3 b = glm::vec3(0);	///< 線分の終点
	};

	/**
	*	カプセル
	*/
	struct Capsule {
		Segment center;	///< 円中部の線分
		float r = 0;	///< カプセルの半径
	};

	/**
	*	凡庸衝突形状
	*/
	struct Shape {
		enum class Type {
			none,	///< 形状無し
			sphere,	///< 球
			capsule,///< カプセル
		};
		Type type = Type::none;	///< 実際の形状
		Sphere s;	///< 弾の形状データ
		Capsule c;	///< カプセルの形状データ
	};

	//形状作成関数
	Shape CreateSphere(const glm::vec3&, float);
	Shape CreateCapsule(const glm::vec3&, const glm::vec3&, float);

	//衝突判定関数
	bool TestSphereSphere(const Sphere&, const Sphere&);
	bool TestSphereCapsule(const Sphere& s, const Capsule& c, glm::vec3* p);
	bool TestShapeShape(const Shape&, const Shape&, glm::vec3* pa, glm::vec3* pb);
}

#endif
