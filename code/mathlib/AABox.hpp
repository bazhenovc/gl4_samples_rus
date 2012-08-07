/**
 e2k engine
 Copyright 2010 e2k team
 */
#ifndef _INC_MATHLIB_AABOX_HPP
#define _INC_MATHLIB_AABOX_HPP

#include <mathlib/Vector3.hpp>

namespace framework
{
/** Axes-aligned bounding box */
struct AABox {
	/// Minimal edge
	Vector3 minEdge;
	/// Maximal edge
	Vector3 maxEdge;

	AABox(void)
		: minEdge(0, 0, 0), maxEdge(0, 0, 0) { }

	AABox(const Vector3& min, const Vector3 & max)
		: minEdge(min), maxEdge(max) { }

	AABox(const Vector3 & point)
		: minEdge(point), maxEdge(point) { }

	AABox(float minx, float miny, float minz,
	      float maxx, float maxy, float maxz)
		: minEdge(minx, miny, minz), maxEdge(maxx, maxy, maxz) { }

	inline bool operator==(const AABox & other) {
		return ( minEdge == other.minEdge && maxEdge == other.maxEdge);
	}

	inline bool operator!=(const AABox & other) {
		return ( minEdge != other.minEdge && maxEdge != other.maxEdge);
	}

	inline AABox operator=(const AABox & box) {
		return AABox(box.minEdge, box.maxEdge);
	}

	void addPoint(const Vector3 & v) {
		addPoint(v.x, v.y, v.z);
	}

	void addPoint(const AABox & other) {
		addPoint(other.minEdge);
		addPoint(other.maxEdge);
	}

	void addPoint(float x, float y, float z) {
		if (x > maxEdge.x) maxEdge.x = x;
		if (y > maxEdge.y) maxEdge.y = y;
		if (z > maxEdge.z) maxEdge.z = z;

		if (x < minEdge.x) minEdge.x = x;
		if (y < minEdge.y) minEdge.y = y;
		if (z < minEdge.z) minEdge.z = z;
	}

	void addBox(const AABox & box) {
		addPoint(box.maxEdge);
		addPoint(box.minEdge);
	}

	void set(const Vector3& min, const Vector3 & max) {
		minEdge = min;
		maxEdge = max;
	}

	void set(const AABox & box) {
		maxEdge = box.maxEdge;
		minEdge = box.minEdge;
	}

	Vector3 getCenter() const {
		return ( maxEdge + minEdge) / 2;
	}

	Vector3 getExtent() const {
		return maxEdge - minEdge;
	}

	float getVolume() const {
		const Vector3 e = getExtent();
		return e.x * e.y * e.z;
	}

	float getArea() const {
		const Vector3 e = getExtent();
		return 2 * (e.x * e.y + e.x * e.z + e.y * e.z);
	}

	bool isEmpty() const {
		return maxEdge == minEdge;
	}

	bool isPointInside(const Vector3 & point) const {
		return ( point.x >= minEdge.x && point.x <= maxEdge.x &&
		         point.y >= minEdge.y && point.y <= maxEdge.y &&
		         point.z >= minEdge.z && point.z <= maxEdge.z);
	}

	bool isPointCompletelyInside(const Vector3 & point) const {
		return ( point.x > minEdge.x && point.x < maxEdge.x &&
		         point.y > minEdge.y && point.y < maxEdge.y &&
		         point.z > minEdge.z && point.z < maxEdge.z);
	}

	void repair() {
		float t;
		if (minEdge.x > maxEdge.x) {
			t = minEdge.x;
			minEdge.x = maxEdge.x;
			maxEdge.x = t;
		}
		if (minEdge.y > maxEdge.y) {
			t = minEdge.y;
			minEdge.y = maxEdge.y;
			maxEdge.y = t;
		}
		if (minEdge.z > maxEdge.z) {
			t = minEdge.z;
			minEdge.z = maxEdge.z;
			maxEdge.z = t;
		}
	}

	Vector3 getVertexP(const Vector3 & normal) {
		Vector3 res = minEdge;

		if (normal.x > 0)
			res.x += maxEdge.x;

		if (normal.y > 0)
			res.y += maxEdge.y;

		if (normal.z > 0)
			res.z += maxEdge.z;

		return ( res);
	}

	Vector3 getVertexN(const Vector3 & normal) {
		Vector3 res = minEdge;

		if (normal.x < 0)
			res.x += maxEdge.x;

		if (normal.y < 0)
			res.y += maxEdge.y;

		if (normal.z < 0)
			res.z += maxEdge.z;

		return ( res);
	}
};
}

#endif
