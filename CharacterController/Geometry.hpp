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

	operator D2D1_POINT_2F() const { return D2D1::Point2F(x, y); }
};

#endif
