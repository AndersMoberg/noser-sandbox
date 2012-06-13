// World.cpp
// Nolan Check
// Created 1/18/2012

#include "World.hpp"

namespace CharacterControllerMode
{

World::World()
{
	// Assemble a few walls for testing
	Wall wall;
	wall.start = Vector2f(0.0f, 5.0f);
	wall.end = Vector2f(5.0f, 0.0f);
	m_walls.push_back(wall);
	wall.start = Vector2f(5.0f, 0.0f);
	wall.end = Vector2f(0.0f, -5.0f);
	m_walls.push_back(wall);
	wall.start = Vector2f(0.0f, -5.0f);
	wall.end = Vector2f(-5.0f, 0.0f);
	m_walls.push_back(wall);

	wall.start = Vector2f(-10.0f, 8.0f);
	wall.end = Vector2f(-9.0f, 0.0f);
	m_walls.push_back(wall);
	wall.start = Vector2f(-9.0f, 0.0f);
	wall.end = Vector2f(-3.0f, -7.0f);
	m_walls.push_back(wall);
}

void World::getCircleContacts(const Vector2f& center, float radius,
	ContactList& contacts)
{
	Contact c;

	for (WallList::const_iterator it = m_walls.begin(); it != m_walls.end(); ++it)
	{
		bool pastS = Vector2f::Dot(it->end - it->start, center - it->start) > 0.0f;
		bool pastE = Vector2f::Dot(it->start - it->end, center - it->end) > 0.0f;
		if (pastS && pastE)
		{
			// center is near some point between the ends
			Vector2f projected = it->start + Vector2f::Projection((center-it->start), (it->end-it->start));
			if ((center - projected).LengthSquared() <= radius*radius)
			{
				c.pos = projected;
				contacts.push_back(c);
			}
		}
		else if (pastS)
		{
			// center is near wall end
			if ((center - it->end).LengthSquared() <= radius*radius)
			{
				c.pos = it->end;
				contacts.push_back(c);
			}
		}
		else if (pastE)
		{
			// center is near wall start
			if ((center - it->start).LengthSquared() <= radius*radius)
			{
				c.pos = it->start;
				contacts.push_back(c);
			}
		}
	}
}

}
