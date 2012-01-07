// Geometry.cpp
// Nolan Check
// Created 1/2/2012

#include "Geometry.hpp"

RectF Matrix3x2f::TransformRect(const RectF& from) const
{
	return RectF(
		m11 * from.left + m31,
		m22 * from.top + m32,
		m11 * from.right + m31,
		m22 * from.bottom + m32);
}

Matrix3x2f Matrix3x2f::RectLerp(const RectF& from, const RectF& to)
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