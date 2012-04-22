// Document.cpp
// Nolan Check
// Created 4/13/2012

#include "Document.hpp"

void Point::setPosition(const Vector2f& pos)
{
	m_pos = pos;
}

void Document::addPoint(const Vector2f& pos)
{
	m_points.push_back(Point(pos));
}

void Document::addWall(Point* pt1, Point* pt2)
{
	m_walls.push_back(Wall(pt1, pt2));
}

Point* Document::findPoint(const Vector2f& pos, float radius)
{
	for (PointList::iterator it = m_points.begin();
		it != m_points.end(); ++it)
	{
		if ((pos - it->getPosition()).lengthSquared() <= radius*radius) {
			return &*it;
		}
	}

	return NULL;
}
