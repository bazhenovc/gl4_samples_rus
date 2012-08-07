#ifndef _INC_MATHLIB_VECTOR2_FPU_HPP
#define _INC_MATHLIB_VECTOR2_FPU_HPP

#include <mathlib/Math.hpp>

namespace framework
{
/**
 Generic 2x2 vector
 */
struct Vector2 {
	union {
		struct {
			float x, y;
		};
		float v[2];
	};

	Vector2(void)
		: x(0), y(0) { }

	explicit
	Vector2(float val)
		: x(val), y(val) { }

	Vector2(float xx, float yy)
		: x(xx), y(yy) { }

	Vector2(const float v[2])
		: x(v[0]), y(v[1]) { }

	Vector2(const Vector2 & other)
		: x(other.x), y(other.y) { }

	inline void set(float xx, float yy) {
		x = xx;
		y = yy;
	}

	inline float* ptr(void) {
		return &v[0];
	}

	inline const float* ptr(void) const {
		return &v[0];
	}

	inline float len(void) const {
		return sqrt(x * x + y * y);
	}

	inline float lenSq(void) const {
		return x * x + y * y;
	}

	inline Vector2 & normalize(void) {
		float inv, length = len();
		if (length < E2K_EPS())
			return *this;
		inv = reciprocal(length);
		x *= inv;
		y *= inv;
		return *this;
	}

	inline float& operator[](size_t i) {
		return v[i];
	}

	inline const float operator[](size_t i) const {
		return v[i];
	}

	inline bool operator==(const Vector2 & v) const {
		return fabs(x - v.x) < E2K_EPS() &&
		       fabs(y - v.y) < E2K_EPS();
	}

	inline bool operator!=(const Vector2 & v) const {
		return !(*this == v);
	}

	inline Vector2 operator-(void) const {
		return Vector2(-x, -y);
	}

	friend Vector2 operator*(const Vector2& v, float f);
	friend Vector2 operator*(const Vector2& v1, const Vector2 & v2);
	friend Vector2 operator/(const Vector2& v, float f);
	friend Vector2 operator/(const Vector2& v1, const Vector2 & v2);
	friend Vector2 operator+(const Vector2& v1, const Vector2 & v2);
	friend Vector2 operator-(const Vector2& v1, const Vector2 & v2);

	inline Vector2 & operator*=(float f) {
		x *= f;
		y *= f;
		return *this;
	}

	inline Vector2 & operator*=(const Vector2 & v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	inline Vector2 & operator/=(float f) {
		float rf = reciprocal(f);
		x *= rf;
		y *= rf;
		return *this;
	}

	inline Vector2 & operator/=(const Vector2 & v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	inline Vector2 & operator+=(const Vector2 & v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	inline Vector2 & operator-=(const Vector2 & v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
};

inline Vector2 operator*(const Vector2& v, float f)
{
	return Vector2(v.x * f, v.y * f);
}

inline Vector2 operator*(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}

inline Vector2 operator/(const Vector2& v, float f)
{
	return Vector2(v.x / f, v.y / f);
}

inline Vector2 operator/(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x / v2.x, v1.y / v2.y);
}

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}
}

#endif
