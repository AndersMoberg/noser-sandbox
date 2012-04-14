// Document.cpp
// Nolan Check
// Created 4/13/2012

#include "Document.hpp"

void Document::addPoint(const Vector2f& pt)
{
	m_points.push_back(pt);
}