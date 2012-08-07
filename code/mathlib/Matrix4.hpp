#ifndef _INC_MATHLIB_MATRIX4_FPU_HPP
#define _INC_MATHLIB_MATRIX4_FPU_HPP

#include <mathlib/AABox.hpp>
#include <mathlib/Vector3.hpp>
#include <mathlib/Vector4.hpp>
#include <mathlib/Matrix3.hpp>

namespace framework
{
/** A 4x4 matrix */
struct Matrix4 {
	float mat[16];

	Matrix4() {
		mat[0] = 1.0;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = 1.0;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = 1.0;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
	}

	Matrix4(const Vector3 & v) {
		translate(v);
	}

	Matrix4(float x, float y, float z) {
		translate(x, y, z);
	}

	Matrix4(const Vector3& axis, float angle) {
		rotate(axis, angle);
	}

	Matrix4(float x, float y, float z, float angle) {
		rotate(x, y, z, angle);
	}

	Matrix4(const Matrix3 & m) {
		mat[0] = m[0];
		mat[4] = m[3];
		mat[8] = m[6];
		mat[12] = 0.0;
		mat[1] = m[1];
		mat[5] = m[4];
		mat[9] = m[7];
		mat[13] = 0.0;
		mat[2] = m[2];
		mat[6] = m[5];
		mat[10] = m[8];
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
	}

	Matrix4(const float* m) {
		mat[0] = m[0];
		mat[4] = m[4];
		mat[8] = m[8];
		mat[12] = m[12];
		mat[1] = m[1];
		mat[5] = m[5];
		mat[9] = m[9];
		mat[13] = m[13];
		mat[2] = m[2];
		mat[6] = m[6];
		mat[10] = m[10];
		mat[14] = m[14];
		mat[3] = m[3];
		mat[7] = m[7];
		mat[11] = m[11];
		mat[15] = m[15];
	}

	Matrix4(const Matrix4 & m) {
		mat[0] = m[0];
		mat[4] = m[4];
		mat[8] = m[8];
		mat[12] = m[12];
		mat[1] = m[1];
		mat[5] = m[5];
		mat[9] = m[9];
		mat[13] = m[13];
		mat[2] = m[2];
		mat[6] = m[6];
		mat[10] = m[10];
		mat[14] = m[14];
		mat[3] = m[3];
		mat[7] = m[7];
		mat[11] = m[11];
		mat[15] = m[15];
	}

	inline Vector3 operator*(const Vector3 & v) const {
		Vector3 ret;
		ret[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12];
		ret[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13];
		ret[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14];
		return ret;
	}

	inline Vector4 operator*(const Vector4 & v) const {
		Vector4 ret;
		ret[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12] * v[3];
		ret[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13] * v[3];
		ret[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14] * v[3];
		ret[3] = mat[3] * v[0] + mat[7] * v[1] + mat[11] * v[2] + mat[15] * v[3];
		return ret;
	}

	inline Matrix4 operator*(float f) const {
		Matrix4 ret;
		ret[0] = mat[0] * f;
		ret[4] = mat[4] * f;
		ret[8] = mat[8] * f;
		ret[12] = mat[12] * f;
		ret[1] = mat[1] * f;
		ret[5] = mat[5] * f;
		ret[9] = mat[9] * f;
		ret[13] = mat[13] * f;
		ret[2] = mat[2] * f;
		ret[6] = mat[6] * f;
		ret[10] = mat[10] * f;
		ret[14] = mat[14] * f;
		ret[3] = mat[3] * f;
		ret[7] = mat[7] * f;
		ret[11] = mat[11] * f;
		ret[15] = mat[15] * f;
		return ret;
	}

	inline Matrix4 operator*(const Matrix4 & m) const {
		Matrix4 ret;
		ret[0] = mat[0] * m[0] + mat[4] * m[1] + mat[8] * m[2] + mat[12] * m[3];
		ret[1] = mat[1] * m[0] + mat[5] * m[1] + mat[9] * m[2] + mat[13] * m[3];
		ret[2] = mat[2] * m[0] + mat[6] * m[1] + mat[10] * m[2] + mat[14] * m[3];
		ret[3] = mat[3] * m[0] + mat[7] * m[1] + mat[11] * m[2] + mat[15] * m[3];
		ret[4] = mat[0] * m[4] + mat[4] * m[5] + mat[8] * m[6] + mat[12] * m[7];
		ret[5] = mat[1] * m[4] + mat[5] * m[5] + mat[9] * m[6] + mat[13] * m[7];
		ret[6] = mat[2] * m[4] + mat[6] * m[5] + mat[10] * m[6] + mat[14] * m[7];
		ret[7] = mat[3] * m[4] + mat[7] * m[5] + mat[11] * m[6] + mat[15] * m[7];
		ret[8] = mat[0] * m[8] + mat[4] * m[9] + mat[8] * m[10] + mat[12] * m[11];
		ret[9] = mat[1] * m[8] + mat[5] * m[9] + mat[9] * m[10] + mat[13] * m[11];
		ret[10] = mat[2] * m[8] + mat[6] * m[9] + mat[10] * m[10] + mat[14] * m[11];
		ret[11] = mat[3] * m[8] + mat[7] * m[9] + mat[11] * m[10] + mat[15] * m[11];
		ret[12] = mat[0] * m[12] + mat[4] * m[13] + mat[8] * m[14] + mat[12] * m[15];
		ret[13] = mat[1] * m[12] + mat[5] * m[13] + mat[9] * m[14] + mat[13] * m[15];
		ret[14] = mat[2] * m[12] + mat[6] * m[13] + mat[10] * m[14] + mat[14] * m[15];
		ret[15] = mat[3] * m[12] + mat[7] * m[13] + mat[11] * m[14] + mat[15] * m[15];
		return ret;
	}

	inline Matrix4 operator+(const Matrix4 & m) const {
		Matrix4 ret;
		ret[0] = mat[0] + m[0];
		ret[4] = mat[4] + m[4];
		ret[8] = mat[8] + m[8];
		ret[12] = mat[12] + m[12];
		ret[1] = mat[1] + m[1];
		ret[5] = mat[5] + m[5];
		ret[9] = mat[9] + m[9];
		ret[13] = mat[13] + m[13];
		ret[2] = mat[2] + m[2];
		ret[6] = mat[6] + m[6];
		ret[10] = mat[10] + m[10];
		ret[14] = mat[14] + m[14];
		ret[3] = mat[3] + m[3];
		ret[7] = mat[7] + m[7];
		ret[11] = mat[11] + m[11];
		ret[15] = mat[15] + m[15];
		return ret;
	}

	inline Matrix4 operator-(const Matrix4 & m) const {
		Matrix4 ret;
		ret[0] = mat[0] - m[0];
		ret[4] = mat[4] - m[4];
		ret[8] = mat[8] - m[8];
		ret[12] = mat[12] - m[12];
		ret[1] = mat[1] - m[1];
		ret[5] = mat[5] - m[5];
		ret[9] = mat[9] - m[9];
		ret[13] = mat[13] - m[13];
		ret[2] = mat[2] - m[2];
		ret[6] = mat[6] - m[6];
		ret[10] = mat[10] - m[10];
		ret[14] = mat[14] - m[14];
		ret[3] = mat[3] - m[3];
		ret[7] = mat[7] - m[7];
		ret[11] = mat[11] - m[11];
		ret[15] = mat[15] - m[15];
		return ret;
	}

	inline Matrix4 & operator*=(float f) {
		return *this = *this * f;
	}

	inline Matrix4 & operator*=(const Matrix4 & m) {
		return *this = *this * m;
	}

	inline Matrix4 & operator+=(const Matrix4 & m) {
		return *this = *this +m;
	}

	inline Matrix4 & operator-=(const Matrix4 & m) {
		return *this = *this -m;
	}

	// ISO C++ says that these are ambigious...
	//inline operator float*() {
	//return mat;
	//}

	//inline operator const float*() const {
	//return mat;
	//}

	inline float* ptr() {
		return mat;
	}

	inline const float* ptr() const {
		return mat;
	}

	inline float &operator[] (size_t i) {
		return mat[i];
	}

	inline const float operator[] (size_t i) const {
		return mat[i];
	}

	inline Matrix4 rotation() const {
		Matrix4 ret;
		ret[0] = mat[0];
		ret[4] = mat[4];
		ret[8] = mat[8];
		ret[12] = 0;
		ret[1] = mat[1];
		ret[5] = mat[5];
		ret[9] = mat[9];
		ret[13] = 0;
		ret[2] = mat[2];
		ret[6] = mat[6];
		ret[10] = mat[10];
		ret[14] = 0;
		ret[3] = 0;
		ret[7] = 0;
		ret[11] = 0;
		ret[15] = 1;
		return ret;
	}

	inline Matrix4 transpose() const {
		Matrix4 ret;
		ret[0] = mat[0];
		ret[4] = mat[1];
		ret[8] = mat[2];
		ret[12] = mat[3];
		ret[1] = mat[4];
		ret[5] = mat[5];
		ret[9] = mat[6];
		ret[13] = mat[7];
		ret[2] = mat[8];
		ret[6] = mat[9];
		ret[10] = mat[10];
		ret[14] = mat[11];
		ret[3] = mat[12];
		ret[7] = mat[13];
		ret[11] = mat[14];
		ret[15] = mat[15];
		return ret;
	}

	inline Matrix4 transposeRotation() const {
		Matrix4 ret;
		ret[0] = mat[0];
		ret[4] = mat[1];
		ret[8] = mat[2];
		ret[12] = mat[12];
		ret[1] = mat[4];
		ret[5] = mat[5];
		ret[9] = mat[6];
		ret[13] = mat[13];
		ret[2] = mat[8];
		ret[6] = mat[9];
		ret[10] = mat[10];
		ret[14] = mat[14];
		ret[3] = mat[3];
		ret[7] = mat[7];
		ret[14] = mat[14];
		ret[15] = mat[15];
		return ret;
	}

	inline float determinant() const {
		float det;
		det = mat[0] * mat[5] * mat[10];
		det += mat[4] * mat[9] * mat[2];
		det += mat[8] * mat[1] * mat[6];
		det -= mat[8] * mat[5] * mat[2];
		det -= mat[4] * mat[1] * mat[10];
		det -= mat[0] * mat[9] * mat[6];
		return det;
	}

	inline Matrix4 inverse() const {
		Matrix4 ret;
		float idet = 1.0f / determinant();
		ret[0] = (mat[5] * mat[10] - mat[9] * mat[6]) * idet;
		ret[1] = -(mat[1] * mat[10] - mat[9] * mat[2]) * idet;
		ret[2] = (mat[1] * mat[6] - mat[5] * mat[2]) * idet;
		ret[3] = 0.0;
		ret[4] = -(mat[4] * mat[10] - mat[8] * mat[6]) * idet;
		ret[5] = (mat[0] * mat[10] - mat[8] * mat[2]) * idet;
		ret[6] = -(mat[0] * mat[6] - mat[4] * mat[2]) * idet;
		ret[7] = 0.0;
		ret[8] = (mat[4] * mat[9] - mat[8] * mat[5]) * idet;
		ret[9] = -(mat[0] * mat[9] - mat[8] * mat[1]) * idet;
		ret[10] = (mat[0] * mat[5] - mat[4] * mat[1]) * idet;
		ret[11] = 0.0;
		ret[12] = -(mat[12] * ret[0] + mat[13] * ret[4] + mat[14] * ret[8]);
		ret[13] = -(mat[12] * ret[1] + mat[13] * ret[5] + mat[14] * ret[9]);
		ret[14] = -(mat[12] * ret[2] + mat[13] * ret[6] + mat[14] * ret[10]);
		ret[15] = 1.0;
		return ret;
	}

	inline void zero() {
		mat[0] = 0.0;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = 0.0;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = 0.0;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 0.0;
	}

	inline void identity() {
		mat[0] = 1.0;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = 1.0;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = 1.0;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
	}

	static inline Matrix4 rotate(const Vector3& axis, float angle) {
		Matrix4 mat;
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
		mat[4] = (1.0f - c) * xy - zs;
		mat[8] = (1.0f - c) * zx + ys;
		mat[12] = 0.0;
		mat[1] = (1.0f - c) * xy + zs;
		mat[5] = (1.0f - c) * yy + c;
		mat[9] = (1.0f - c) * yz - xs;
		mat[13] = 0.0;
		mat[2] = (1.0f - c) * zx - ys;
		mat[6] = (1.0f - c) * yz + xs;
		mat[10] = (1.0f - c) * zz + c;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 rotate(float x, float y, float z, float angle) {
		return rotate(Vector3(x, y, z), angle);
	}

	static inline Matrix4 rotateX(float angle) {
		Matrix4 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = 1.0;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = c;
		mat[9] = -s;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = s;
		mat[10] = c;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 rotateY(float angle) {
		Matrix4 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = c;
		mat[4] = 0.0;
		mat[8] = s;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = 1.0;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = -s;
		mat[6] = 0.0;
		mat[10] = c;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 rotateZ(float angle) {
		Matrix4 mat;
		float rad = angle * DEG2RAD;
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = c;
		mat[4] = -s;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = s;
		mat[5] = c;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = 1.0;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 scale(const Vector3 & v) {
		Matrix4 mat;
		mat[0] = v.x;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = v.y;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = v.z;
		mat[14] = 0.0;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 scale(float x, float y, float z) {
		return scale(Vector3(x, y, z));
	}

	static inline Matrix4 translate(const Vector3 & v) {
		Matrix4 mat;
		mat[0] = 1.0;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = v.x;
		mat[1] = 0.0;
		mat[5] = 1.0;
		mat[9] = 0.0;
		mat[13] = v.y;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = 1.0;
		mat[14] = v.z;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 translate(float x, float y, float z) {
		return translate(Vector3(x, y, z));
	}

	static inline Matrix4 reflect(const Vector4 & plane) {
		Matrix4 mat;
		float x = plane.x;
		float y = plane.y;
		float z = plane.z;
		float x2 = x * 2.0f;
		float y2 = y * 2.0f;
		float z2 = z * 2.0f;
		mat[0] = 1.0f - x * x2;
		mat[4] = -y * x2;
		mat[8] = -z * x2;
		mat[12] = -plane.w * x2;
		mat[1] = -x * y2;
		mat[5] = 1.0f - y * y2;
		mat[9] = -z * y2;
		mat[13] = -plane.w * y2;
		mat[2] = -x * z2;
		mat[6] = -y * z2;
		mat[10] = 1.0f - z * z2;
		mat[14] = -plane.w * z2;
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = 0.0;
		mat[15] = 1.0;
		return mat;
	}

	static inline Matrix4 reflect(float x, float y, float z, float w) {
		return reflect(Vector4(x, y, z, w));
	}

	static inline Matrix4 perspective(float fov, float aspect, float znear, float zfar) {
		Matrix4 mat;
		float y = tan(fov * PI / 360.0f);
		float x = y * aspect;
		mat[0] = 1.0f / x;
		mat[4] = 0.0;
		mat[8] = 0.0;
		mat[12] = 0.0;
		mat[1] = 0.0;
		mat[5] = 1.0f / y;
		mat[9] = 0.0;
		mat[13] = 0.0;
		mat[2] = 0.0;
		mat[6] = 0.0;
		mat[10] = -(zfar + znear) / (zfar - znear);
		mat[14] = -(2.0f * zfar * znear) / (zfar - znear);
		mat[3] = 0.0;
		mat[7] = 0.0;
		mat[11] = -1.0;
		mat[15] = 0.0;
		return mat;
	}

	static inline Matrix4 lookAt(const Vector3& pos, const Vector3& dir, const Vector3 & up) {
		Matrix4 mat;
		Vector3 x, y, z;
		Matrix4 m0, m1;
		z = pos - dir;
		z.normalize();
		x.cross(up, z);
		x.normalize();
		y.cross(z, x);
		y.normalize();
		m0[0] = x.x;
		m0[4] = x.y;
		m0[8] = x.z;
		m0[12] = 0.0;
		m0[1] = y.x;
		m0[5] = y.y;
		m0[9] = y.z;
		m0[13] = 0.0;
		m0[2] = z.x;
		m0[6] = z.y;
		m0[10] = z.z;
		m0[14] = 0.0;
		m0[3] = 0.0;
		m0[7] = 0.0;
		m0[11] = 0.0;
		m0[15] = 1.0;
		m1.translate(-pos);
		mat = m0 * m1;
		return mat;
	}

	inline AABox transformBox(const AABox & box) const {
		AABox ret = box;
		ret.minEdge += Vector3(mat[12], mat[13], mat[14]);
		ret.maxEdge += Vector3(mat[12], mat[13], mat[14]);
		return ret;
	}

};
}

#endif
