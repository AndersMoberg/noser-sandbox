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

class Wall
{

public:

	Wall(Point* pt1, Point* pt2)
	{
		m_pts[0] = pt1;
		m_pts[1] = pt2;
	}

	Point* getPoint(bool dir) const {
		return m_pts[dir ? 1 : 0];
	}

private:

	Point* m_pts[2];

};

class Document
{

public:

	typedef std::list<Point> PointList;
	typedef std::list<Wall> WallList;

	void addPoint(const Vector2f& pos);
	void addWall(Point* pt1, Point* pt2);

	Point* findPoint(const Vector2f& pos, float radius);

	const PointList& getPoints() const { return m_points; }
	const WallList& getWalls() const { return m_walls; }

private:

	PointList m_points;
	WallList m_walls;

};

#endif
