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

Matrix4x4f Matrix4x4f::scale(const Vector3f& v)
{
	return Matrix4x4f(
		v.x, 0.0f, 0.0f, 0.0f,
		0.0f, v.y, 0.0f, 0.0f,
		0.0f, 0.0f, v.z, 0.0f,
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

// Ref: <http://www.cprogramming.com/tutorial/3d/quaternions.html>
Quaternionf Quaternionf::operator*(const Quaternionf& rhs) const
{
	return Quaternionf(
		w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y,
		w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x,
		w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w,
		w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z);
}

void Quaternionf::normalize()
{
	float l2 = x*x + y*y + z*z + w*w;
	if (l2 != 1.0f)
	{
		float l = sqrt(l2);
		x /= l;
		y /= l;
		z /= l;
		w /= l;
	}
}

// Ref: <http://www.cprogramming.com/tutorial/3d/quaternions.html>
Matrix4x4f Quaternionf::getMatrix() const
{
	// Please make sure the quaternion is normalized first
	return Matrix4x4f(
		// row 1
		1.0f - 2.0f*y*y - 2.0f*z*z,
		2.0f*x*y - 2.0f*w*z,
		2.0f*x*z + 2.0f*w*y,
		0.0f,
		// row 2
		2.0f*x*y + 2.0f*w*z,
		1.0f - 2.0f*x*x - 2.0f*z*z,
		2.0f*y*z + 2.0f*w*x,
		0.0f,
		// row 3
		2.0f*x*z - 2.0f*w*y,
		2.0f*y*z - 2.0f*w*x,
		1.0f - 2.0f*x*x - 2.0f*y*y,
		0.0f,
		// row 4
		0.0f, 0.0f, 0.0f, 1.0f);
}
