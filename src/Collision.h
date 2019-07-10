/**
*	@file Collision.h
*/
#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Collision {

	/**
	*	�e
	*/
	struct Sphere {
		Sphere(glm::vec3 p, float r) :center(p), r(r) {}
		Sphere() = default;
		glm::vec3 center = glm::vec3(0);///< ���S���W
		float r = 0; ///< ���a
	};

	bool TestSphereSphere(const Sphere&, const Sphere&);
}

#endif
