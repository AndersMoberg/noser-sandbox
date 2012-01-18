// World.cpp
// Nolan Check
// Created 1/18/2012

#include "World.hpp"

World::World()
{ }

WorldPtr World::Create()
{
	WorldPtr p(new World);

	return p;
}
