// Geometry.hpp
// Nolan Check
// Created 3/16/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

struct Rectf
{
	float left;
	float top;
	float right;
	float bottom;

	Rectf(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b)
	{ }
};

#endif
