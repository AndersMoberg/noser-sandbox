// WorldRenderer.cpp
// Nolan Check
// Created 5/29/2012

#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer()
{ }

std::unique_ptr<WorldRenderer> WorldRenderer::create(GLES2Renderer* renderer)
{
	std::unique_ptr<WorldRenderer> p(new WorldRenderer);

	p->m_objects.push_back(WorldObject::create(
		renderer, L"TestObject.png", Rectf(-2.0f, 2.0f, 2.0f, -2.0f)));

	return p;
}

void WorldRenderer::render()
{
	for (ObjectList::const_iterator it = m_objects.begin();
		it != m_objects.end(); ++it)
	{
		(*it)->render();
	}
}
