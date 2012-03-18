// Geometry.cpp
// Nolan Check
// Created 1/20/2012

#include "Geometry.hpp"

const Matrix3x2f Matrix3x2f::IDENTITY(
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f);

Matrix3x2f Matrix3x2f::RectLerp(const Rectf& from, const Rectf& to)
{
	float m11 = (to.right - to.left) / (from.right - from.left);
	float m22 = (to.top - to.bottom) / (from.top - from.bottom);
	float m31 = to.left - from.left * m11;
	float m32 = to.top - from.top * m22;
	return Matrix3x2f(
		m11, 0.0f,
		0.0f, m22,
		m31, m32);
}
