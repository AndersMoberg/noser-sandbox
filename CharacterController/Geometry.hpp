// Geometry.hpp
// Nolan Check
// Created 1/18/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <d2d1.h>

struct Vector2f
{
	float x;
	float y;

	Vector2f() { }
	Vector2f(float _x, float _y) : x(_x), y(_y) { }

	Vector2f operator-(const Vector2f& rhs) const { return Vector2f(x - rhs.x, y - rhs.y); }
	Vector2f operator*(float rhs) const { return Vector2f(x * rhs, y * rhs); }

	Vector2f& operator+=(const Vector2f& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	operator D2D1_POINT_2F() const { return D2D1::Point2F(x, y); }

	float LengthSquared() const { return x * x + y * y; }
	Vector2f Perpendicular() const { return Vector2f(-y, x); }

	// Return the dot product of a and b.
	// Equivalent to |a|*|b|*cos(angle)
	static float Dot(const Vector2f& a, const Vector2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	// Return the vector projection of a onto b.
	static Vector2f Projection(const Vector2f& a, const Vector2f& b)
	{
		float l2 = b.LengthSquared();
		if (l2 != 0.0f) {
			return b * (Dot(a, b) / l2);
		} else {
			return a;
		}
	}
};

inline Vector2f operator*(float lhs, const Vector2f& rhs) {
	return Vector2f(lhs * rhs.x, lhs * rhs.y);
}

static float DistancePointLineSquared(const Vector2f& p, const Vector2f& l1, const Vector2f& l2)
{
	float c = Vector2f::Dot((l2 - l1).Perpendicular(), p - l1);
	return c*c / (l2 - l1).LengthSquared();
}

struct Rectf
{
	float left;
	float top;
	float right;
	float bottom;

	Rectf() { }
	Rectf(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b)
	{ }
};

struct Matrix3x2f
{
	float m11;
	float m12;
	float m21;
	float m22;
	float m31;
	float m32;

	Matrix3x2f() { }
	Matrix3x2f(float _11, float _12, float _21, float _22, float _31, float _32)
		: m11(_11), m12(_12), m21(_21), m22(_22), m31(_31), m32(_32)
	{ }

	operator D2D1_MATRIX_3X2_F() const { return D2D1::Matrix3x2F(m11, m12, m21, m22, m31, m32); }

	static Matrix3x2f RectLerp(const Rectf& from, const Rectf& to);
};

#endif
