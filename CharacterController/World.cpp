// World.cpp
// Nolan Check
// Created 1/18/2012

#include "World.hpp"

World::World()
{ }

WorldPtr World::Create()
{
	WorldPtr p(new World);

	// Assemble a few walls for testing
	Wall wall;
	wall.start = Vector2f(5.0f, 5.0f);
	wall.end = Vector2f(11.0f, 1.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(11.0f, 1.0f);
	wall.end = Vector2f(-5.0f, -3.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(-5.0f, -3.0f);
	wall.end = Vector2f(-11.0f, 1.0f);
	p->m_walls.push_back(wall);

	return p;
}
