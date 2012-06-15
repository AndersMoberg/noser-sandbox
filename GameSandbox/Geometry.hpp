// Geometry.hpp
// Nolan Check
// Created 1/18/2012

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <cmath>

#include <d2d1.h>

static const float M_PIf = 3.14159265358979323846f;

inline float DegToRad(float deg) {
	return deg * M_PIf / 180.0f;
}

struct Vector2f
{
	float x;
	float y;

	Vector2f() { }
	Vector2f(float _x, float _y) : x(_x), y(_y) { }

	Vector2f operator+(const Vector2f& rhs) const { return Vector2f(x + rhs.x, y + rhs.y); }
	Vector2f operator-(const Vector2f& rhs) const { return Vector2f(x - rhs.x, y - rhs.y); }
	Vector2f operator*(float rhs) const { return Vector2f(x * rhs, y * rhs); }
	Vector2f operator/(float rhs) const { return Vector2f(x / rhs, y / rhs); }

	bool operator==(const Vector2f& rhs) const { return x == rhs.x && y == rhs.y; }

	Vector2f& operator+=(const Vector2f& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vector2f& operator-=(const Vector2f& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	operator D2D1_POINT_2F() const { return D2D1::Point2F(x, y); }

	float LengthSquared() const { return x * x + y * y; }
	float length() const { return sqrt(LengthSquared()); }
	Vector2f Perpendicular() const { return Vector2f(-y, x); }
	bool isZero() const { return x == 0.0f && y == 0.0f; }
	Vector2f normalize() const { return *this / length(); }

	// Return the dot product of a and b.
	// Equivalent to |a|*|b|*cos(angle)
	static float Dot(const Vector2f& a, const Vector2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	// Return the vector projection of a onto b.
	static Vector2f Projection(const Vector2f& a, const Vector2f& b)
	{
		float l2 = b.LengthSquared();
		if (l2 != 0.0f) {
			return b * (Dot(a, b) / l2);
		} else {
			return a;
		}
	}
};

inline Vector2f operator*(float lhs, const Vector2f& rhs) {
	return Vector2f(lhs * rhs.x, lhs * rhs.y);
}

static float DistancePointLineSquared(const Vector2f& p, const Vector2f& l1, const Vector2f& l2)
{
	float c = Vector2f::Dot((l2 - l1).Perpendicular(), p - l1);
	return c * c / (l2 - l1).LengthSquared();
}

struct Rectf
{
	float left;
	float top;
	float right;
	float bottom;

	Rectf() { }
	Rectf(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b)
	{ }

	operator D2D1_RECT_F() const { return D2D1::RectF(left, top, right, bottom); }

	Vector2f UpperLeft() const { return Vector2f(left, top); }

	Rectf Offset(const Vector2f& v) {
		return Rectf(left + v.x, top + v.y, right + v.x, bottom + v.y);
	}

};

struct Matrix3x2f
{
	float _11;
	float _12;
	float _21;
	float _22;
	float _31;
	float _32;

	Matrix3x2f() { }
	Matrix3x2f(float m11, float m12, float m21, float m22, float m31, float m32)
		: _11(m11), _12(m12), _21(m21), _22(m22), _31(m31), _32(m32)
	{ }

	// Matrix multiplication: This matrix is post-multiplied by rhs, and rhs
	// is pre-multiplied by this matrix.
	Matrix3x2f operator*(const Matrix3x2f& rhs) const
	{
		return Matrix3x2f(
			_11 * rhs._11 + _12 * rhs._21,
			_11 * rhs._12 + _12 * rhs._22,
			_21 * rhs._11 + _22 * rhs._21,
			_21 * rhs._12 + _22 * rhs._22,
			_31 * rhs._11 + _32 * rhs._21 + rhs._31,
			_31 * rhs._12 + _32 * rhs._22 + rhs._32);
	}

	operator D2D1_MATRIX_3X2_F() const { return D2D1::Matrix3x2F(_11, _12, _21, _22, _31, _32); }

	Vector2f transform(const Vector2f& v) const
	{
		return Vector2f(
			_11 * v.x + _12 * v.y + _31,
			_21 * v.x + _22 * v.y + _32);
	}

	static Matrix3x2f Scale(const Vector2f& scale, const Vector2f& center = Vector2f(0.0f, 0.0f))
	{
		return Matrix3x2f(
			scale.x, 0.0f,
			0.0f, scale.y,
			center.x - scale.x * center.x, center.y - scale.y * center.y);
	}
	static Matrix3x2f Scale(float x, float y, const Vector2f& center = Vector2f(0.0f, 0.0f)) {
		return Scale(Vector2f(x, y), center);
	}

	static Matrix3x2f RectLerp(const Rectf& from, const Rectf& to);

	static const Matrix3x2f IDENTITY;
};

#endif
