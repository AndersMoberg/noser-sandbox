// World.cpp
// Nolan Check
// Created 1/18/2012

#include "World.hpp"

namespace CharacterControllerMode
{

World::World()
{ }

WorldPtr World::Create()
{
	WorldPtr p(new World);

	// Assemble a few walls for testing
	Wall wall;
	wall.start = Vector2f(0.0f, 5.0f);
	wall.end = Vector2f(5.0f, 0.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(5.0f, 0.0f);
	wall.end = Vector2f(0.0f, -5.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(0.0f, -5.0f);
	wall.end = Vector2f(-5.0f, 0.0f);
	p->m_walls.push_back(wall);

	wall.start = Vector2f(-10.0f, 8.0f);
	wall.end = Vector2f(-9.0f, 0.0f);
	p->m_walls.push_back(wall);
	wall.start = Vector2f(-9.0f, 0.0f);
	wall.end = Vector2f(-3.0f, -7.0f);
	p->m_walls.push_back(wall);

	return p;
}

}
