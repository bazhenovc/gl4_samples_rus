#ifndef _INC_MATHLIB_MATRIX3_FPU_HPP
#define _INC_MATHLIB_MATRIX3_FPU_HPP

#include <mathlib/Vector3.hpp>
#include <mathlib/Vector4.hpp>

namespace framework
{
/**
 A 3x3 matrix. Right handed coordinate system
 */
struct Matrix3 {
	float mat[9];

	Matrix3(void) {
		mat[0] = 1.0;
		mat[3] = 0.0;
		mat[6] = 0.0;
		mat[1] = 0.0;
		mat[4] = 1.0;
		mat[7] = 0.0;
		mat[2] = 0.0;
		mat[5] = 0.0;
		mat[8] = 1.0;
	}

	Matrix3(float m[9]) {
		mat[0] = m[0];
		mat[3] = m[3];
		mat[6] = m[6];
		mat[1] = m[1];
		mat[4] = m[4];
		mat[7] = m[7];
		mat[2] = m[2];
		mat[5] = m[5];
		mat[8] = m[8];
	}

	Matrix3(const Matrix3 & m) {
		mat[0] = m.mat[0];
		mat[3] = m.mat[3];
		mat[6] = m.mat[6];
		mat[1] = m.mat[1];
		mat[4] = m.mat[4];
		mat[7] = m.mat[7];
		mat[2] = m.mat[2];
		mat[5] = m.mat[5];
		mat[8] = m.mat[8];
	}

	inline Vector3 operator*(const Vector3 & v) const {
		Vector3 ret;
		ret.x = mat[0] * v.x + mat[3] * v.y + mat[6] * v.z;
		ret.y = mat[1] * v.x + mat[4] * v.y + mat[7] * v.z;
		ret.z = mat[2] * v.x + mat[5] * v.y + mat[8] * v.z;
		return ret;
	}

	inline Vector4 operator*(const Vector4 & v) const {
		Vector4 ret;
		ret.x = mat[0] * v.x + mat[3] * v.y + mat[6] * v.z;
		ret.y = mat[1] * v.x + mat[4] * v.y + mat[7] * v.z;
		ret.z = mat[2] * v.x + mat[5] * v.y + mat[8] * v.z;
		ret.w = v.w;
		return ret;
	}

	inline Matrix3 operator*(float f) const {
		Matrix3 ret;
		ret.mat[0] = mat[0] * f;
		ret.mat[3] = mat[3] * f;
		ret.mat[6] = mat[6] * f;
		ret.mat[1] = mat[1] * f;
		ret.mat[4] = mat[4] * f;
		ret.mat[7] = mat[7] * f;
		ret.mat[2] = mat[2] * f;
		ret.mat[5] = mat[5] * f;
		ret.mat[8] = mat[8] * f;
		return ret;
	}

	inline Matrix3 operator*(const Matrix3& m) const {
		Matrix3 ret;
		ret.mat[0] = mat[0] * m.mat[0] + mat[3] * m.mat[1] + mat[6] * m.mat[2];
		ret.mat[1] = mat[1] * m.mat[0] + mat[4] * m.mat[1] + mat[7] * m.mat[2];
		ret.mat[2] = mat[2] * m.mat[0] + mat[5] * m.mat[1] + mat[8] * m.mat[2];
		ret.mat[3] = mat[0] * m.mat[3] + mat[3] * m.mat[4] + mat[6] * m.mat[5];
		ret.mat[4] = mat[1] * m.mat[3] + mat[4] * m.mat[4] + mat[7] * m.mat[5];
		ret.mat[5] = mat[2] * m.mat[3] + mat[5] * m.mat[4] + mat[8] * m.mat[5];
		ret.mat[6] = mat[0] * m.mat[6] + mat[3] * m.mat[7] + mat[6] * m.mat[8];
		ret.mat[7] = mat[1] * m.mat[6] + mat[4] * m.mat[7] + mat[7] * m.mat[8];
		ret.mat[8] = mat[2] * m.mat[6] + mat[5] * m.mat[7] + mat[8] * m.mat[8];
		return ret;
	}

	inline Matrix3 operator+(const Matrix3& m) const {
		Matrix3 ret;
		ret.mat[0] = mat[0] + m.mat[0];
		ret.mat[3] = mat[3] + m.mat[3];
		ret.mat[6] = mat[6] + m.mat[6];
		ret.mat[1] = mat[1] + m.mat[1];
		ret.mat[4] = mat[4] + m.mat[4];
		ret.mat[7] = mat[7] + m.mat[7];
		ret.mat[2] = mat[2] + m.mat[2];
		ret.mat[5] = mat[5] + m.mat[5];
		ret.mat[8] = mat[8] + m.mat[8];
		return ret;
	}

	inline Matrix3 operator-(const Matrix3& m) const {
		Matrix3 ret;
		ret.mat[0] = mat[0] - m.mat[0];
		ret.mat[3] = mat[3] - m.mat[3];
		ret.mat[6] = mat[6] - m.mat[6];
		ret.mat[1] = mat[1] - m.mat[1];
		ret.mat[4] = mat[4] - m.mat[4];
		ret.mat[7] = mat[7] - m.mat[7];
		ret.mat[2] = mat[2] - m.mat[2];
		ret.mat[5] = mat[5] - m.mat[5];
		ret.mat[8] = mat[8] - m.mat[8];
		return ret;
	}

	inline Matrix3& operator*=(float f) {
		return *this = *this * f;
	}

	inline Matrix3& operator*=(const Matrix3& m) {
		return *this = *this * m;
	}

	inline Matrix3& operator+=(const Matrix3& m) {
		return *this = *this +m;
	}

	inline Matrix3& operator-=(const Matrix3& m) {
		return *this = *this -m;
	}

	//operator float*() {
	//return mat;
	//}

	//operator const float*() const {
	//return mat;
	//}

	inline float* ptr() {
		return mat;
	}

	inline const float* ptr() const {
		return mat;
	}

	inline float& operator[] (size_t i) {
		return mat[i];
	}

	inline const float operator[] (size_t i) const {
		return mat[i];
	}

	inline Matrix3 transpose() const {
		Matrix3 ret;
		ret.mat[0] = mat[0];
		ret.mat[3] = mat[1];
		ret.mat[6] = mat[2];
		ret.mat[1] = mat[3];
		ret.mat[4] = mat[4];
		ret.mat[7] = mat[5];
		ret.mat[2] = mat[6];
		ret.mat[5] = mat[7];
		ret.mat[8] = mat[8];
		return ret;
	}

	inline float determinant() const {
		float det;
		det = mat[0] * mat[4] * mat[8];
		det += mat[3] * mat[7] * mat[2];
		det += mat[6] * mat[1] * mat[5];
		det -= mat[6] * mat[4] * mat[2];
		det -= mat[3] * mat[1] * mat[8];
		det -= mat[0] * mat[7] * mat[5];
		return det;
	}

	inline Matrix3 inverse() const {
		Matrix3 ret;
		float idet = 1.0f / determinant();
		ret.mat[0] = (mat[4] * mat[8] - mat[7] * mat[5]) * idet;
		ret.mat[1] = -(mat[1] * mat[8] - mat[7] * mat[2]) * idet;
		ret.mat[2] = (mat[1] * mat[5] - mat[4] * mat[2]) * idet;
		ret.mat[3] = -(mat[3] * mat[8] - mat[6] * mat[5]) * idet;
		ret.mat[4] = (mat[0] * mat[8] - mat[6] * mat[2]) * idet;
		ret.mat[5] = -(mat[0] * mat[5] - mat[3] * mat[2]) * idet;
		ret.mat[6] = (mat[3] * mat[7] - mat[6] * mat[4]) * idet;
		ret.mat[7] = -(mat[0] * mat[7] - mat[6] * mat[1]) * idet;
		ret.mat[8] = (mat[0] * mat[4] - mat[3] * mat[1]) * idet;
		return ret;
	}

	inline void zero() {
		mat[0] = 0.0;
		mat[3] = 0.0;
		mat[6] = 0.0;
		mat[1] = 0.0;
		mat[4] = 0.0;
		mat[7] = 0.0;
		mat[2] = 0.0;
		mat[5] = 0.0;
		mat[8] = 0.0;
	}

	inline void identity() {
		mat[0] = 1.0;
		mat[3] = 0.0;
		mat[6] = 0.0;
		mat[1] = 0.0;
		mat[4] = 1.0;
		mat[7] = 0.0;
		mat[2] = 0.0;
		mat[5] = 0.0;
		mat[8] = 1.0;
	}

	static inline Matrix3
	rotate(const Vector3& axis, float angle) {
		Matrix3 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		Vector3 v = axis;
		v.normalize();
		float xx = v.x * v.x;
		float yy = v.y * v.y;
		float zz = v.z * v.z;
		float xy = v.x * v.y;
		float yz = v.y * v.z;
		float zx = v.z * v.x;
		float xs = v.x * s;
		float ys = v.y * s;
		float zs = v.z * s;
		mat[0] = (1.0f - c) * xx + c;
		mat[3] = (1.0f - c) * xy - zs;
		mat[6] = (1.0f - c) * zx + ys;
		mat[1] = (1.0f - c) * xy + zs;
		mat[4] = (1.0f - c) * yy + c;
		mat[7] = (1.0f - c) * yz - xs;
		mat[2] = (1.0f - c) * zx - ys;
		mat[5] = (1.0f - c) * yz + xs;
		mat[8] = (1.0f - c) * zz + c;
		return mat;
	}

	static inline Matrix3 rotate(float x, float y, float z, float angle) {
		return rotate(Vector3(x, y, z), angle);
	}

	static inline Matrix3 rotateX(float angle) {
		Matrix3 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = 1.0;
		mat[3] = 0.0;
		mat[6] = 0.0;
		mat[1] = 0.0;
		mat[4] = c;
		mat[7] = -s;
		mat[2] = 0.0;
		mat[5] = s;
		mat[8] = c;
		return mat;
	}

	static inline Matrix3 rotateY(float angle) {
		Matrix3 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = c;
		mat[3] = 0.0;
		mat[6] = s;
		mat[1] = 0.0;
		mat[4] = 1.0;
		mat[7] = 0.0;
		mat[2] = -s;
		mat[5] = 0.0;
		mat[8] = c;
		return mat;
	}

	static inline Matrix3 rotateZ(float angle) {
		Matrix3 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = c;
		mat[3] = -s;
		mat[6] = 0.0;
		mat[1] = s;
		mat[4] = c;
		mat[7] = 0.0;
		mat[2] = 0.0;
		mat[5] = 0.0;
		mat[8] = 1.0;
		return mat;
	}

	static inline Matrix3 scale(const Vector3 & v) {
		Matrix3 mat;
		mat[0] = v.x;
		mat[3] = 0.0;
		mat[6] = 0.0;
		mat[1] = 0.0;
		mat[4] = v.y;
		mat[7] = 0.0;
		mat[2] = 0.0;
		mat[5] = 0.0;
		mat[8] = v.z;
		return mat;
	}

	static inline Matrix3 scale(float x, float y, float z) {
		return scale(Vector3(x, y, z));
	}

	inline void orthonormalize() {
		Vector3 x(mat[0], mat[1], mat[2]);
		Vector3 y(mat[3], mat[4], mat[5]);
		Vector3 z;
		x.normalize();
		z.cross(x, y);
		z.normalize();
		y.cross(z, x);
		y.normalize();
		mat[0] = x.x;
		mat[3] = y.x;
		mat[6] = z.x;
		mat[1] = x.y;
		mat[4] = y.y;
		mat[7] = z.y;
		mat[2] = x.z;
		mat[5] = y.z;
		mat[8] = z.z;
	}
};
}

#endif
