// Geometry.hpp
// Nolan Check
// Created 4/13/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <wx/wx.h>

struct Vector2f
{
	float x;
	float y;

	Vector2f() { }
	Vector2f(float _x, float _y) : x(_x), y(_y) { }

	Vector2f operator-(const Vector2f& rhs) const {
		return Vector2f(x - rhs.x, y - rhs.y);
	}

	float lengthSquared() const { return x*x + y*y; }

	operator wxPoint2DDouble() const {
		return wxPoint2DDouble(x, y);
	}

};

#endif
