// World.cpp
// Nolan Check
// Created 1/18/2012

#include "World.hpp"

World::World()
{ }

WorldPtr World::Create()
{
	WorldPtr p(new World);

	// Assemble a couple walls for testing
	Wall wall;
	wall.start = Vector2f(-1.0f, 5.0f);
	wall.end = Vector2f(5.0f, 1.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(-8.0f, -4.0f);
	wall.end = Vector2f(8.0f, 2.0f);
	p->m_walls.push_back(wall);

	return p;
}
