/**
 e2k engine
 Copyright 2010 e2k team
 */
#ifndef _INC_MATHLIB_MATH_HPP
#define _INC_MATHLIB_MATH_HPP

#include <stdlib.h>
#include <math.h>
#include <limits>

#define E2K_EPS std::numeric_limits<float>::epsilon

#ifndef M_PI
#define PI 3.14159265358979323846f
#else
#define PI M_PI
#endif

#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

#define IEEE_1   0x3f800000
#define IEEE_255 0x437f0000

#define E2K_SQR(X) (X * X)

namespace framework
{

/**
 Reciprocal value
 */
//  inline float reciprocal(float x)
//  {
//#ifndef __SSE__
//    return 1.0f / x;
//#else
//    // SSE Newton-Raphson reciprocal estimate
//    float rec;
//    asm(""
//              " rcpss xmm0, f"
//              " movss xmm1, f"
//              " mulss xmm1, xmm0"
//              " mulss xmm1, xmm0"
//              " addss xmm0, xmm0"
//              " subss xmm0, xmm1"
//              " movss rec,  xmm0"
//              );
//    return rec;
//#endif
//  }
#define reciprocal(X) 1 / X

/**
 Reciprocal square root
 */
inline float rsqrt(const float x)
{
	unsigned tmp = (unsigned(IEEE_1 << 1) + IEEE_1 - *(unsigned*) &x) >> 1;
	float y = *(float*) &tmp;
	return y * (1.47f - 0.47f * x * y * y);
}

/**
 Check whether given value is NaN
 */
template <typename T>
inline bool isNaN(const T t)
{
	return t != t;
}

/**
 Check whether integer is power of two
 */
inline bool isPowerOf2(const int v)
{
	return v && !(v & (v - 1));
}
}

#endif