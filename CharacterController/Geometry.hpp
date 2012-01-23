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

	Vector2f& operator+=(const Vector2f& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	operator D2D1_POINT_2F() const { return D2D1::Point2F(x, y); }
};

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
