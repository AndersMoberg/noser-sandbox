// Model.hpp
// Nolan Check
// Created 5/30/2012

#ifndef _MODEL_HPP
#define _MODEL_HPP

#include <memory>
#include <string>
#include <vector>

#include "Geometry.hpp"

class Model
{

public:

	typedef std::vector<Vector3f> Points;
	typedef std::vector<std::pair<int,int> > Lines;

	static std::unique_ptr<Model> loadFromFile(const std::wstring& path);
	
	const Points& getPoints() const { return m_points; }
	const Lines& getLines() const { return m_lines; }
	const Boxf& getBounds() const { return m_bounds; }

private:

	Model();

	Points m_points;
	Lines m_lines;
	Boxf m_bounds;

};

#endif
