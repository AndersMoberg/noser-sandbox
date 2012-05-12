// Geometry.hpp
// Nolan Check
// Created 5/9/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#undef near
#undef far

const float M_PIf = 3.1415926535897932384626433832795f;
const float M_2PIf = 6.283185307179586476925286766559f;

struct Vector3f
{
	float x;
	float y;
	float z;

	Vector3f() { }
	Vector3f(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z)
	{ }
};

struct Boxf
{
	float left;
	float top;
	float near;
	float right;
	float bottom;
	float far;

	Boxf() { }
	Boxf(float l, float t, float n, float r, float b, float f)
		: left(l), top(t), near(n), right(r), bottom(b), far(f)
	{ }
};

struct Matrix4x4f
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float f[16];
	};

	Matrix4x4f() { }
	Matrix4x4f(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44)
		: _11(m11), _12(m12), _13(m13), _14(m14),
		_21(m21), _22(m22), _23(m23), _24(m24),
		_31(m31), _32(m32), _33(m33), _34(m34),
		_41(m41), _42(m42), _43(m43), _44(m44)
	{ }

	Matrix4x4f operator*(const Matrix4x4f& rhs) const;

	static const Matrix4x4f IDENTITY;

	static Matrix4x4f boxLerp(const Boxf& from, const Boxf& to);
	static Matrix4x4f rotateX(float angle);
	static Matrix4x4f rotateY(float angle);
};

#endif
