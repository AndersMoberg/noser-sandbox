// Document.hpp
// Nolan Check
// Created 4/13/2012

#ifndef _DOCUMENT_HPP
#define _DOCUMENT_HPP

#include <list>

#include "Geometry.hpp"

class Point
{

public:

	Point(const Vector2f& pos)
		: m_pos(pos)
	{ }

	const Vector2f& getPosition() const { return m_pos; }
	void setPosition(const Vector2f& pos);

private:

	Vector2f m_pos;

};

class Document
{

public:

	typedef std::list<Point> PointList;

	void addPoint(const Vector2f& pos);

	Point* findPoint(const Vector2f& pos, float radius);

	const PointList& getPoints() const { return m_points; }

private:

	PointList m_points;

};

#endif
