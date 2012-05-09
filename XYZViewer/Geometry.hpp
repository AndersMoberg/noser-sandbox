// Geometry.hpp
// Nolan Check
// Created 5/9/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

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

#endif
