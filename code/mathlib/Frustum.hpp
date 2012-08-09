/**
 e2k engine
 Copyright 2010 e2k team
 */
#ifndef _INC_MATHLIB_FRUSTUM_HPP
#define	_INC_MATHLIB_FRUSTUM_HPP

#include <mathlib/glm::mat4.hpp>
#include <mathlib/Vector3.hpp>
#include <mathlib/Vector4.hpp>

namespace framework
{
enum Side {
    RIGHT = 0, // The RIGHT side of the frustum
    LEFT = 1, // The LEFT	 side of the frustum
    BOTTOM = 2, // The BOTTOM side of the frustum
    TOP = 3, // The TOP side of the frustum
    BACK = 4, // The BACK	side of the frustum
    FRONT = 5, // The FRONT side of the frustum
    A = 0, // The X value of the plane's normal
    B = 1, // The Y value of the plane's normal
    C = 2, // The Z value of the plane's normal
    D = 3 // The distance the plane is from the origin
};
enum Intersect {
    OUTSIDE = 0,
    INTERSECT,
    INSIDE
};

/** Frustum */
/// TODO: methods should return a distance to the object
struct Frustum {
	glm::vec4 planes[6];

	Frustum(void) { }

	Intersect boxInFrustum(AABox & aabb) const {
		glm::vec3 center = aabb.getCenter(),
		        halfDiag = aabb.maxEdge;
		float m, n;
		m = n = 0.0f;
		halfDiag -= center;
		for (size_t i = 0; i < 6; i++) {
			m = (center.x * planes[i].x) + (center.y * planes[i].y) +
			    (center.z * planes[i].z) + planes[i].w;
			n = (halfDiag.x * fabs(planes[i].x)) + (halfDiag.y * fabs(planes[i].y)) +
			    (halfDiag.z * fabs(planes[i].z));
			if (m + n < 0) {
				return OUTSIDE;
			}
		}
		return INSIDE;
	}

	void compute(const glm::mat4& proj, const glm::mat4 & modl) {
		glm::vec4 *p;
		glm::mat4 clip = proj * modl;
		p = &planes[RIGHT];
		p->x = clip[3] - clip[0];
		p->y = clip[7] - clip[4];
		p->z = clip[11] - clip[8];
		p->w = clip[15] - clip[12];

		p = &planes[LEFT];
		p->x = clip[3] + clip[0];
		p->y = clip[7] + clip[4];
		p->z = clip[11] + clip[8];
		p->w = clip[15] + clip[12];

		p = &planes[BOTTOM];
		p->x = clip[3] + clip[1];
		p->y = clip[7] + clip[5];
		p->z = clip[11] + clip[9];
		p->w = clip[15] + clip[13];

		p = &planes[TOP];
		p->x = clip[3] - clip[1];
		p->y = clip[7] - clip[5];
		p->z = clip[11] - clip[9];
		p->w = clip[15] - clip[13];

		p = &planes[BACK];
		p->x = clip[3] - clip[2];
		p->y = clip[7] - clip[6];
		p->z = clip[11] - clip[10];
		p->w = clip[15] - clip[14];

		p = &planes[FRONT];
		p->x = clip[3] + clip[2];
		p->y = clip[7] + clip[6];
		p->z = clip[11] + clip[10];
		p->w = clip[15] + clip[14];

		float mag = 0.0f;
		for (size_t i = 0; i < 6; i++) {
			mag = sqrt(planes[i].x * planes[i].x +
			           planes[i].y * planes[i].y +
			           planes[i].z * planes[i].z);
			if (mag)
				planes[i] /= mag;
		}
	}
};
}

#endif	/* FRUSTUM_HPP */

