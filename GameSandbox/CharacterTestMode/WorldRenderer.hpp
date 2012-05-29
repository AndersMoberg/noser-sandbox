// WorldRenderer.hpp
// Nolan Check
// Created 5/27/2012

#ifndef _WORLDRENDERER_HPP
#define _WORLDRENDERER_HPP

#include <list>

#include "WorldObject.hpp"

class WorldRenderer
{

public:

	static std::unique_ptr<WorldRenderer> create(GLES2Renderer* renderer);

	void render();

private:

	WorldRenderer();

	typedef std::list<std::unique_ptr<WorldObject> > ObjectList;

	ObjectList m_objects;

};

#endif
