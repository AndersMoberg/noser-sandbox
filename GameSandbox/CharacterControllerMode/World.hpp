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

struct Contact
{
	Vector2f pos;
};

typedef std::list<Contact> ContactList;

class World
{

public:

	World();

	typedef std::list<Wall> WallList;

	const WallList& GetWalls() const { return m_walls; }

	void getCircleContacts(const Vector2f& center, float radius,
		ContactList& contacts);

private:

	WallList m_walls;

};

}

#endif
