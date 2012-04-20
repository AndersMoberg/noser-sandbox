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
