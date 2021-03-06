// Geometry.hpp
// Nolan Check
// Created 5/9/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <cmath>

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

	Vector3f operator+(const Vector3f& rhs) const { return Vector3f(x+rhs.x, y+rhs.y, z+rhs.z); }
	Vector3f operator-(const Vector3f& rhs) const { return Vector3f(x-rhs.x, y-rhs.y, z-rhs.z); }
	Vector3f operator-() const { return Vector3f(-x, -y, -z); }
	Vector3f operator/(float rhs) const { return Vector3f(x/rhs, y/rhs, z/rhs); }

	float lengthSquared() const { return x*x + y*y + z*z; }
	float length() const { return sqrt(lengthSquared()); }

	void normalize()
	{
		float l = length();
		x /= l;
		y /= l;
		z /= l;
	}

	Vector3f arbitraryPerpendicular() const
	{
		if (abs(x) <= abs(y) && abs(x) <= abs(z)) { // x is lowest magnitude
			return cross(*this, Vector3f(1.0f, 0.0f, 0.0f));
		} else if (abs(y) <= abs(x) && abs(y) <= abs(z)) { // y is lowest magnitude
			return cross(*this, Vector3f(0.0f, 1.0f, 0.0f));
		} else { // z is lowest magnitude
			return cross(*this, Vector3f(0.0f, 0.0f, 1.0f));
		}
	}

	static Vector3f cross(const Vector3f& a, const Vector3f& b)
	{
		return Vector3f(
			a.y*b.z - b.y*a.z,
			a.z*b.x - a.x*b.z,
			a.x*b.y - b.x*a.y
			);
	}
};

inline Vector3f operator*(float lhs, const Vector3f& rhs) {
	return Vector3f(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);
}

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

	Vector3f center() const { return Vector3f((left+right)/2.0f, (top+bottom)/2.0f, (near+far)/2.0f); }
};

// Components are stored in row-major order. NOTE: GLES requires column-major!
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
	static Matrix4x4f scale(const Vector3f& v);
	static Matrix4x4f translate(const Vector3f& v);
	static Matrix4x4f rotateX(float angle);
	static Matrix4x4f rotateY(float angle);
};

struct Quaternionf
{
	float x;
	float y;
	float z;
	float w;

	Quaternionf() { }
	Quaternionf(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w)
	{ }

	Quaternionf operator*(const Quaternionf& rhs) const;

	void normalize();
	Matrix4x4f getMatrix() const;
};

#endif
