/**
*	@file Collision.cpp
*/
#include "Collision.h"

namespace Collision{

	/**
	*	�e�ƒe���Փ˂��Ă��邩���ׂ�
	*
	*	@param s0	����Ώۂ̋�����1
	*	@param s1	����Ώۂ̋�����2
	*
	*	@return true	�Փ˂��Ă���
	*	@return false	�Փ˂��Ă��Ȃ�
	*/
	bool TestSphereSphere(const Sphere& s0, const Sphere& s1) {

		const glm::vec3 m = s0.center - s1.center;
		const float radiusSum = s0.r + s1.r;
		return glm::dot(m, m) <= radiusSum * radiusSum;
	}


}