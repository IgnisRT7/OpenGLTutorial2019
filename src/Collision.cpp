/**
*	@file Collision.cpp
*/
#include "Collision.h"
#include <iostream>

namespace Collision{

	/**
	*	球形状を作成する
	*
	*	@param center	球の中心座標
	*	@param r		球の半径	
	*
	*	@return 球を保持する凡庸衝突形状オブジェクト
	*/
	Shape CreateSphere(const glm::vec3& center, float r) {

		Shape result;
		result.type = Shape::Type::sphere;
		result.s = Sphere{ center,r };
		return result;	
	}

	/**
	*	カプセル形状を作成する
	*
	*	@param a	中心の線分の始点座標
	*	@param b	中心の線分の終点座標
	*	@param r	カプセルの半径
	*
	*	@return カプセルを保持する凡庸衝突形状オブジェクト
	*/
	Shape CreateCapsule(const glm::vec3& a, const glm::vec3& b, float r) {

		Shape result;
		result.type = Shape::Type::capsule;
		result.c = Capsule{ { a, b },r };
		return result;
	}

	/**
	*	弾と弾が衝突しているか調べる
	*
	*	@param s0	判定対象の球その1
	*	@param s1	判定対象の球その2
	*
	*	@return true	衝突している
	*	@return false	衝突していない
	*/
	bool TestSphereSphere(const Sphere& s0, const Sphere& s1) {

		const glm::vec3 m = s0.center - s1.center;
		const float radiusSum = s0.r + s1.r;
		return glm::dot(m, m) <= radiusSum * radiusSum;
	}

	/**
	*	線分と点の最近接点を調べる
	*
	*	@param seg	線分
	*	@param p	点
	*
	*	@retrun segとpの最近接点
	*/
	glm::vec3 ClosestPointSegment(const Segment& seg, const glm::vec3& p){
		
		const glm::vec3 ab = seg.b - seg.a;
		const glm::vec3 ap = p - seg.a;
		const float lenAQ = glm::dot(ab, ap);
		const float lenAB = glm::dot(ab, ab);

		if (lenAQ <= 0) {
			return seg.a;
		}
		else if (lenAQ >= lenAB) {
			return seg.b;
		}

		return seg.a + ab * (lenAQ / lenAB);
	}

	/**
	*	球とカプセルが衝突しているか調べる
	*	
	*	@param s	球
	*	@param c	カプセル
	*	@param p	最近接点の格納先
	*
	*	@retval true	衝突している
	*	@retval false	衝突していない
	*/
	bool TestSphereCapsule(const Sphere& s, const Capsule& c, glm::vec3* p){

		*p = ClosestPointSegment(c.seg, s.center);
		const glm::vec3 distance = *p - s.center;
		const float radiusSum = s.r + c.r;
		return glm::dot(distance, distance) <= radiusSum * radiusSum;
	}

	/**
	*	シェイプ同士が衝突しているか調べる
	*
	*	@param a	判定対象のシェイプその1
	*	@param b	判定対象のシェイプその2
	*	@param pa	衝突した座標	
	*	@param pb	衝突した座標
	*
	*	@retval true	衝突した
	*	@retval false	衝突しなかった
	*/
	bool TestShapeShape(const Shape& a, const Shape& b, glm::vec3* pa, glm::vec3* pb) {

		if (a.type == Shape::Type::sphere) {
			if (b.type == Shape::Type::sphere) {
				if (TestSphereSphere(a.s, b.s)) {
					*pa = a.s.center;
					*pb = b.s.center;
					return true;
				}
			}
			else if (b.type == Shape::Type::capsule) {
				if (TestSphereCapsule(a.s,b.c,pb)) {
					*pa = a.s.center;
					return true;
				}
			}
		}
		else if (a.type == Shape::Type::capsule) {
			if (b.type == Shape::Type::sphere) {
				if (TestSphereCapsule(b.s, a.c, pa)) {
					*pb = b.s.center;
					return true;
				}
			}
			else if(b.type == Shape::Type::capsule) {
				std::cout << __func__ << "[警告]: カプセルとカプセルの衝突判定は対応外です\n";
			}
		}
		return false;
	}
}