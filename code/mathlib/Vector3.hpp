#ifndef _INC_MATHLIB_VECTOR3_FPU_HPP
#define _INC_MATHLIB_VECTOR3_FPU_HPP

#include <mathlib/Math.hpp>

namespace framework
{
/**
 3D vector
 */
struct Vector3 {
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};

	Vector3(void)
		: x(0.0f), y(0.0f), z(0.0f) { }

	explicit
	Vector3(float v)
		: x(v), y(v), z(v) { }

	Vector3(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) { }

	Vector3(const float v[3])
		: x(v[0]), y(v[1]), z(v[2]) { }

	Vector3(const Vector3 & other)
		: x(other.x), y(other.y), z(other.z) { }

	inline void set(float xx, float yy, float zz) {
		x = xx;
		y = yy;
		z = zz;
	}

	float* ptr(void) {
		return &v[0];
	}

	const float* ptr(void) const {
		return &v[0];
	}

	inline float len(void) const {
		return sqrt(x * x + y * y + z * z);
	}

	inline float lenSq(void) const {
		return x * x + y * y + z * z;
	}

	inline Vector3 & normalize(void) {
		float inv, length = len();
		if (length < E2K_EPS())
			return *this;
		inv = reciprocal(length);
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	}

	inline bool operator==(const Vector3 & v) const {
		return fabs(x - v.x) < E2K_EPS() &&
		       fabs(y - v.y) < E2K_EPS() &&
		       fabs(z - v.z) < E2K_EPS();
	}

	inline bool operator!=(const Vector3 & v) const {
		return !(*this == v);
	}

	inline Vector3 operator-(void) const {
		return Vector3(-x, -y, -z);
	}

	inline float& operator[](size_t i) {
		return v[i];
	}

	inline const float operator[](size_t i) const {
		return v[i];
	}

	inline Vector3 & operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	inline Vector3 & operator*=(const Vector3 & v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	inline Vector3 & operator/=(float f) {
		float rf = reciprocal(f);
		x *= rf;
		y *= rf;
		z *= rf;
		return *this;
	}

	inline Vector3 & operator/=(const Vector3 & v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	inline Vector3 & operator+=(const Vector3 & v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector3 & operator-=(const Vector3 & v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	friend Vector3 operator*(const Vector3& v, float f);
	friend Vector3 operator*(const Vector3& v1, const Vector3 & v2);
	friend Vector3 operator/(const Vector3& v, float f);
	friend Vector3 operator/(const Vector3& v1, const Vector3 & v2);
	friend Vector3 operator+(const Vector3& v1, const Vector3 & v2);
	friend Vector3 operator-(const Vector3& v1, const Vector3 & v2);
	friend Vector3 cross(const Vector3& v1, const Vector3 & v2);

	inline void cross(const Vector3& v1, const Vector3 & v2) {
		x = v1.y * v2.z - v1.z * v2.y;
		y = v1.z * v2.x - v1.x * v2.z;
		z = v1.x * v2.y - v1.y * v2.x;
	}
};

inline Vector3 operator*(const Vector3& v, float f)
{
	return Vector3(v.x * f, v.y * f, v.z * f);
}

inline Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline Vector3 operator/(const Vector3& v, float f)
{
	return Vector3(v.x / f, v.y / f, v.z / f);
}

inline Vector3 operator/(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vector3 cross(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.y * v2.z - v1.z * v2.y,
	               v1.z * v2.x - v1.x * v2.z,
	               v1.x * v2.y - v1.y * v2.x);
}
}

#endif
