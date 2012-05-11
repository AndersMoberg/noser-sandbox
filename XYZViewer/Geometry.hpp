// Geometry.hpp
// Nolan Check
// Created 5/9/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

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

#endif
