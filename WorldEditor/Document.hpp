// Document.hpp
// Nolan Check
// Created 4/13/2012

#ifndef _DOCUMENT_HPP
#define _DOCUMENT_HPP

#include <vector>

#include "Geometry.hpp"

class Document
{

public:

	void addPoint(const Vector2f& pt);

	const std::vector<Vector2f>& getPoints() const { return m_points; }

private:

	std::vector<Vector2f> m_points;

};

#endif
