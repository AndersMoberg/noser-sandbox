// Model.cpp
// Nolan Check
// Created 5/30/2012

#include "Model.hpp"

#include <fstream>
#include <sstream>

Model::Model()
{ }

std::unique_ptr<Model> Model::loadFromFile(const std::wstring& path)
{
	std::unique_ptr<Model> p(new Model);
	
	std::ifstream ifs(path);
	bool boundsInited = false;
	std::string line;
	int pointIndex = 0;
	bool makeLine = false;
	while (ifs)
	{
		std::string line;
		std::getline(ifs, line);
		if (!line.empty())
		{
			if (line[0] == '#') {
				makeLine = false;
				continue;
			}

			std::istringstream iss(line);
			float x, y, z;
			iss >> x >> y >> z;
			if (iss)
			{
				p->m_points.push_back(Vector3f(x, y, z));
				if (!boundsInited)
				{
					p->m_bounds.left = p->m_bounds.right = x;
					p->m_bounds.top = p->m_bounds.bottom = y;
					p->m_bounds.near = p->m_bounds.far = z;
					boundsInited = true;
				}
				else
				{
					p->m_bounds.left = std::min(x, p->m_bounds.left);
					p->m_bounds.top = std::max(y, p->m_bounds.top);
					p->m_bounds.near = std::min(z, p->m_bounds.near);
					p->m_bounds.right = std::max(x, p->m_bounds.right);
					p->m_bounds.bottom = std::min(y, p->m_bounds.bottom);
					p->m_bounds.far = std::max(z, p->m_bounds.far);
				}

				if (makeLine)
				{
					p->m_lines.push_back(std::make_pair(pointIndex-1, pointIndex));
				}
				++pointIndex;
				makeLine = true;
			}
			else
			{
				makeLine = false;
			}
		}
		else
		{
			makeLine = false;
		}
	}

	return p;
}
