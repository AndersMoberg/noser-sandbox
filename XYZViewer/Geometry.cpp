// Geometry.cpp
// Nolan Check
// Created 5/11/2012

#include "Geometry.hpp"

const Matrix4x4f Matrix4x4f::IDENTITY(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

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
