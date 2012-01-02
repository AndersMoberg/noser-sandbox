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

#endif
