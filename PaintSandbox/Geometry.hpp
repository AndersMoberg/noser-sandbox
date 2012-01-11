// Geometry.hpp
// Nolan Check
// Created 12/30/2011

#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <cmath>

template<class T>
struct Vector2
{
	T x;
	T y;

	Vector2() { }
	Vector2(T _x, T _y) : x(_x), y(_y) { }

	Vector2 operator-(const Vector2& rhs) const {
		return Vector2(x - rhs.x, y - rhs.y);
	}

	T LengthSquared() const { return x*x + y*y; }
	float Length() const { return sqrt(LengthSquared()); }
};

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;

template<class T>
struct Rect
{
	T left;
	T top;
	T right;
	T bottom;

	Rect() { }
	Rect(T l, T t, T r, T b)
		: left(l), top(t), right(r), bottom(b)
	{ }

	Vector2<T> UpperLeft() const { return Vector2<T>(left, top); }
	Vector2<T> LowerRight() const { return Vector2<T>(right, bottom); }
	Vector2<T> Center() const { return Vector2<T>((left+right)/2.0f, (bottom+top)/2.0f); }
};

typedef Rect<float> RectF;
typedef Rect<int> RectI;

struct Matrix3x2f
{
	float m11, m12;
	float m21, m22;
	float m31, m32;

	Matrix3x2f() { }
	Matrix3x2f(float _m11, float _m12, float _m21, float _m22, float _m31, float _m32)
		: m11(_m11), m12(_m12), m21(_m21), m22(_m22), m31(_m31), m32(_m32)
	{ }

	Vector2f TransformPoint(const Vector2f& from) const {
		return Vector2f(m11*from.x + m12*from.y + m31,
			m21*from.x + m22*from.y + m32);
	};

	static Matrix3x2f RectLerp(const RectF& from, const RectF& to);

	static const Matrix3x2f IDENTITY;
};

inline float Lerp(float x0, float x1, float y0, float y1, float x) {
	return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

inline RectF LerpRect(float x0, float x1, const RectF& y0, const RectF& y1, float x)
{
	return RectF(Lerp(x0, x1, y0.left, y1.left, x),
		Lerp(x0, x1, y0.top, y1.top, x),
		Lerp(x0, x1, y0.right, y1.right, x),
		Lerp(x0, x1, y0.bottom, y1.bottom, x));
}

#endif
