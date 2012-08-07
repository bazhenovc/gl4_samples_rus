#ifndef _INC_MATHLIB_VECTOR4_FPU_HPP
#define _INC_MATHLIB_VECTOR4_FPU_HPP

#include <mathlib/Math.hpp>
#include <mathlib/Vector3.hpp>

namespace framework
{
/**
 4D vector
 */
struct Vector4 {
	union {
		struct {
			float x, y, z, w;
		};
		float v[4];
	};

	Vector4(void)
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }

	explicit
	Vector4(float v)
		: x(v), y(v), z(v), w(v) { }

	Vector4(float xx, float yy, float zz, float ww = 1.0f)
		: x(xx), y(yy), z(zz), w(ww) { }

	Vector4(float v[4])
		: x(v[0]), y(v[1]), z(v[2]), w(v[3]) { }

	explicit
	Vector4(const Vector3 & other)
		: x(other.x), y(other.y), z(other.z), w(1.0f) { }

	Vector4(const Vector4 & other)
		: x(other.x), y(other.y), z(other.z), w(other.w) { }

	inline void set(float xx, float yy, float zz, float ww = 1.0f) {
		x = xx;
		y = yy;
		z = zz;
		w = ww;
	}

	inline float* ptr(void) {
		return &v[0];
	}

	inline const float* ptr(void) const {
		return &v[0];
	}

	inline float& operator[](size_t i) {
		return v[i];
	}

	inline const float operator[](size_t i) const {
		return v[i];
	}

	inline bool operator==(const Vector4 & v) const {
		return fabs(x - v.x) < E2K_EPS() &&
		       fabs(y - v.y) < E2K_EPS() &&
		       fabs(z - v.z) < E2K_EPS();
	}

	inline bool operator!=(const Vector4 & v) const {
		return !(*this == v);
	}

	inline Vector4 operator-(void)const {
		return Vector4(-x, -y, -z, -w);
	}

	friend Vector4 operator*(const Vector4& v, float f);
	friend Vector4 operator*(const Vector4& v1, const Vector4 & v2);
	friend Vector4 operator/(const Vector4& v, float f);
	friend Vector4 operator/(const Vector4& v1, const Vector4 & v2);
	friend Vector4 operator+(const Vector4& v1, const Vector4 & v2);
	friend Vector4 operator-(const Vector4& v1, const Vector4 & v2);

	inline Vector4 & operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	inline Vector4 & operator*=(const Vector4 & v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	inline Vector4 & operator/=(float f) {
		float rf = reciprocal(f);
		x *= rf;
		y *= rf;
		z *= rf;
		w *= rf;
		return *this;
	}

	inline Vector4 & operator/=(const Vector4 & v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	inline Vector4 & operator+=(const Vector4 & v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	inline Vector4 & operator-=(const Vector4 & v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
};

inline Vector4 operator*(const Vector4& v, float f)
{
	return Vector4(v.x * f, v.y * f, v.z * f, v.w * f);
}

inline Vector4 operator*(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

inline Vector4 operator/(const Vector4& v, float f)
{
	return Vector4(v.x / f, v.y / f, v.z / f, v.w / f);
}

inline Vector4 operator/(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

inline Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}
}

#endif
