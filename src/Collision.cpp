/**
*	@file Collision.cpp
*/
#include "Collision.h"

namespace Collision{

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


}