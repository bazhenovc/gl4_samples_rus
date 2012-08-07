#ifndef _INC_MATHLIB_QUATERNION_FPU_HPP
#define _INC_MATHLIB_QUATERNION_FPU_HPP

#include <mathlib/Vector3_FPU.hpp>
#include <mathlib/Matrix3_FPU.hpp>
#include <mathlib/Matrix4_FPU.hpp>

namespace framework
{
/** Quaternion. Used to represent rotations, as it`s very fast to
 * interpolate him. */
struct Quaternion {
	union {
		struct {
			float x, y, z, w;
		};
		float q[4];
	};

	Quaternion()
		: x(0), y(0), z(0), w(1) { }

	Quaternion(const _Vector3& dir, float angle) {
		set(dir, angle);
	}

	Quaternion(float x, float y, float z, float angle) {
		set(x, y, z, angle);
	}

	Quaternion(const _Matrix3 & m) {
		float trace = m[0] + m[4] + m[8];
		if (trace > 0.0) {
			float s = sqrt(trace + 1.0f);
			q[3] = 0.5f * s;
			s = 0.5f / s;
			q[0] = (m[5] - m[7]) * s;
			q[1] = (m[6] - m[2]) * s;
			q[2] = (m[1] - m[3]) * s;
		} else {
			static int next[3] = {1, 2, 0};
			int i = 0;
			if (m[4] > m[0]) i = 1;
			if (m[8] > m[3 * i + i]) i = 2;
			int j = next[i];
			int k = next[j];
			float s = sqrt(m[3 * i + i] - m[3 * j + j] - m[3 * k + k] + 1.0f);
			q[i] = 0.5f * s;
			if (s != 0) s = 0.5f / s;
			q[3] = (m[3 * j + k] - m[3 * k + j]) * s;
			q[j] = (m[3 * i + j] + m[3 * j + i]) * s;
			q[k] = (m[3 * i + k] + m[3 * k + i]) * s;
		}
	}

	Quaternion operator-() const {
		return Quaternion(-x, -y, -z, -w);
	}

	//        inline operator float*()
	//        {
	//            return (float*) &x;
	//        }
	//
	//        inline operator const float*() const
	//        {
	//            return (float*) &x;
	//        }

	inline float* ptr() {
		return (float*) &x;
	}

	inline const float* ptr() const {
		return (float*) &x;
	}

	inline float& operator[] (unsigned char i) {
		return q[i];
	}

	inline const float operator[] (unsigned char i) const {
		return q[i];
	}

	inline Quaternion operator*(const Quaternion & q) const {
		Quaternion ret;
		ret.x = w * q.x + x * q.x + y * q.z - z * q.y;
		ret.y = w * q.y + y * q.w + z * q.x - x * q.z;
		ret.z = w * q.z + z * q.w + x * q.y - y * q.x;
		ret.w = w * q.w - x * q.x - y * q.y - z * q.z;
		return ret;
	}

	Quaternion operator+(const Quaternion & q) const {
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	Quaternion operator*(float a) const {
		return Quaternion(x * a, y * a, z * a, w * a);
	}

	inline _Vector3 operator*(const _Vector3 & p) const {
		// original code from DooM 3 SDK
		float xxzz = x * x - z*z;
		float wwyy = w * w - y*y;

		float xw2 = x * w * 2.0f;
		float xy2 = x * y * 2.0f;
		float xz2 = x * z * 2.0f;
		float yw2 = y * w * 2.0f;
		float yz2 = y * z * 2.0f;
		float zw2 = z * w * 2.0f;

		return _Vector3(
		           (xxzz + wwyy) * p.x + (xy2 + zw2) * p.y + (xz2 - yw2) * p.z,
		           (xy2 - zw2) * p.x + (y * y + w * w - x * x - z * z) * p.y + (yz2 + xw2) * p.z,
		           (xz2 + yw2) * p.x + (yz2 - xw2) * p.y + (wwyy - xxzz) * p.z
		       );
	}

	inline void set(const _Vector3& dir, float angle) {
		float length = dir.len();
		if (length != 0.0) {
			length = 1.0f / length;
			float sinangle = sin(angle * DEG2RAD / 2.0f);
			x = dir[0] * length * sinangle;
			y = dir[1] * length * sinangle;
			z = dir[2] * length * sinangle;
			w = cos(angle * DEG2RAD / 2.0f);
		} else {
			x = y = z = 0.0;
			w = 1.0;
		}
	}

	inline void set(float x, float y, float z, float angle) {
		set(_Vector3(x, y, z), angle);
	}

	inline void slerp(const Quaternion& q0, const Quaternion& q1, float t) {
		float k0, k1,
		      cosomega = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
		Quaternion q;
		if (cosomega < 0.0) {
			cosomega = -cosomega;
			q.x = -q1.x;
			q.y = -q1.y;
			q.z = -q1.z;
			q.w = -q1.w;
		} else {
			q.x = q1.x;
			q.y = q1.y;
			q.z = q1.z;
			q.w = q1.w;
		}
		if (1.0 - cosomega > 1e-6) {
			float omega = acos(cosomega);
			float sinomega = sin(omega);
			k0 = sin((1.0f - t) * omega) / sinomega;
			k1 = sin(t * omega) / sinomega;
		} else {
			k0 = 1.0f - t;
			k1 = t;
		}
		x = q0.x * k0 + q.x * k1;
		y = q0.y * k0 + q.y * k1;
		z = q0.z * k0 + q.z * k1;
		w = q0.w * k0 + q.w * k1;
	}

	inline _Matrix3 toMatrix() const {
		_Matrix3 ret;
		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;
		float xx = x * x2;
		float yy = y * y2;
		float zz = z * z2;
		float xy = x * y2;
		float yz = y * z2;
		float xz = z * x2;
		float wx = w * x2;
		float wy = w * y2;
		float wz = w * z2;
		ret[0] = 1.0f - (yy + zz);
		ret[3] = xy - wz;
		ret[6] = xz + wy;
		ret[1] = xy + wz;
		ret[4] = 1.0f - (xx + zz);
		ret[7] = yz - wx;
		ret[2] = xz - wy;
		ret[5] = yz + wx;
		ret[8] = 1.0f - (xx + yy);
		return ret;
	}

	inline _Matrix4 toMatrix4() const {
		_Matrix4 pMatrix;
		pMatrix[ 0] = 1.0f - 2.0f * (y * y + z * z);
		pMatrix[ 1] = 2.0f * (x * y + z * w);
		pMatrix[ 2] = 2.0f * (x * z - y * w);
		pMatrix[ 3] = 0.0f;
		pMatrix[ 4] = 2.0f * (x * y - z * w);
		pMatrix[ 5] = 1.0f - 2.0f * (x * x + z * z);
		pMatrix[ 6] = 2.0f * (z * y + x * w);
		pMatrix[ 7] = 0.0f;
		pMatrix[ 8] = 2.0f * (x * z + y * w);
		pMatrix[ 9] = 2.0f * (y * z - x * w);
		pMatrix[10] = 1.0f - 2.0f * (x * x + y * y);
		pMatrix[11] = 0.0f;
		pMatrix[12] = 0;
		pMatrix[13] = 0;
		pMatrix[14] = 0;
		pMatrix[15] = 1.0f;
		return pMatrix;
	}

	Quaternion inverse() const {
		float lenSq = x * x + y * y + z * z + w * w;
		if (lenSq > 0) {
			/// TODO: rapprox here
			float invLenSq = 1 / lenSq;
			return Quaternion(-x * invLenSq, -y * invLenSq, -z * invLenSq, w * invLenSq);
		} else {
			return Quaternion(0, 0, 0, 1);
		}
	}

	Quaternion & invert() {
		return *this = inverse();
	}

	Quaternion & normalize() {
		float len = (float) sqrt(x * x + y * y + z * z + w * w);
		if (len) {
			/// TODO: rapprox here
			float ilength = 1 / len;
			x *= ilength;
			y *= ilength;
			z *= ilength;
			w *= ilength;
		}
		return ( *this);
	}
};

inline Quaternion slerp(const Quaternion& from, const Quaternion& to, float t)
{
	Quaternion temp;
	float omega, cosom, sinom, scale0, scale1;

	if (t <= 0.0f)
		return from;
	else if (t >= 1.0f)
		return to;

	cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
	if (cosom < 0.0f) {
		temp = -to;
		cosom = -cosom;
	} else
		temp = to;

	if ((1.0f - cosom) > E2K_EPS()) {
		omega = acosf(cosom);
		sinom = 1.0f / sinf(omega);
		scale0 = sinf((1.0f - t) * omega) * sinom;
		scale1 = sinf(t * omega) * sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	return ( (from * scale0) + (temp * scale1));
}
}

#endif
