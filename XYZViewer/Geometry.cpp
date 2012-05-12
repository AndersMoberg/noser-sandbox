// Geometry.cpp
// Nolan Check
// Created 5/11/2012

#include "Geometry.hpp"

#include <cmath>

const Matrix4x4f Matrix4x4f::IDENTITY(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Matrix4x4f Matrix4x4f::operator*(const Matrix4x4f& rhs) const
{
	Matrix4x4f result;
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			result.f[4*j+i] = f[4*j] * rhs.f[i] +
				f[4*j+1] * rhs.f[4+i] +
				f[4*j+2] * rhs.f[8+i] +
				f[4*j+3] * rhs.f[12+i];
		}
	}
	return result;
}

Matrix4x4f Matrix4x4f::boxLerp(const Boxf& from, const Boxf& to)
{
	// Scaling
	float m11 = (to.right - to.left) / (from.right - from.left);
	float m22 = (to.top - to.bottom) / (from.top - from.bottom);
	float m33 = (to.far - to.near) / (from.far - from.near);
	// Translation
	float m14 = to.left - from.left * m11;
	float m24 = to.top - from.top * m22;
	float m34 = to.near - from.near * m33;

	return Matrix4x4f(
		m11, 0.0f, 0.0f, m14,
		0.0f, m22, 0.0f, m24,
		0.0f, 0.0f, m33, m34,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4f Matrix4x4f::translate(const Vector3f& v)
{
	return Matrix4x4f(
		1.0f, 0.0f, 0.0f, v.x,
		0.0f, 1.0f, 0.0f, v.y,
		0.0f, 0.0f, 1.0f, v.z,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4f Matrix4x4f::rotateX(float angle)
{
	float c = cos(angle);
	float s = sin(angle);

	return Matrix4x4f(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,    c,   -s, 0.0f,
		0.0f,    s,    c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4f Matrix4x4f::rotateY(float angle)
{
	float c = cos(angle);
	float s = sin(angle);

	return Matrix4x4f(
		   c, 0.0f,    s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		  -s, 0.0f,    c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}
