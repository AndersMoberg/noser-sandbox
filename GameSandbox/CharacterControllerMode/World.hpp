// World.hpp
// Nolan Check
// Created 1/18/2012

#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <list>
#include <memory>

#include "Geometry.hpp"

namespace CharacterControllerMode
{

struct Wall
{
	Vector2f start;
	Vector2f end;
};

class World
{

public:

	World();

	typedef std::list<Wall> WallList;

	const WallList& GetWalls() const { return m_walls; }

private:

	WallList m_walls;

};

}

#endif
