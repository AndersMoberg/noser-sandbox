// Geometry.hpp
// Nolan Check
// Created 12/30/2011

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

struct Vector2f
{
	float x;
	float y;

	Vector2f() { }
	Vector2f(float _x, float _y) : x(_x), y(_y) { }
};

struct RectF
{
	float left;
	float top;
	float right;
	float bottom;

	RectF() { }
	RectF(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b)
	{ }

	Vector2f UpperLeft() const { return Vector2f(left, top); }
	Vector2f LowerRight() const { return Vector2f(right, bottom); }
};

struct Matrix3x2f
{
	float m11, m12;
	float m21, m22;
	float m31, m32;

	Matrix3x2f() { }
	Matrix3x2f(float _m11, float _m12, float _m21, float _m22, float _m31, float _m32)
		: m11(_m11), m12(_m12), m21(_m21), m22(_m22), m31(_m31), m32(_m32)
	{ }

	RectF TransformRect(const RectF& from) const;
	Vector2f TransformPoint(const Vector2f& from) const {
		return Vector2f(m11*from.x + m12*from.y + m31,
			m21*from.x + m22*from.y + m32);
	};

	static Matrix3x2f RectLerp(const RectF& from, const RectF& to);
};

#endif
