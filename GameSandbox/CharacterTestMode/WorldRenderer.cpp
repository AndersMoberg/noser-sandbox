// WorldRenderer.cpp
// Nolan Check
// Created 5/29/2012

#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer()
{ }

std::unique_ptr<WorldRenderer> WorldRenderer::create(GLES2Renderer::Ptr renderer)
{
	std::unique_ptr<WorldRenderer> p(new WorldRenderer);

	p->m_renderer = renderer;

	p->addObject(L"TestObject.png", Rectf(-2.0f, 2.0f, 2.0f, -2.0f));

	return p;
}

void WorldRenderer::render()
{
	ObjectList sorted = m_objects;
	class SortByY
	{
	public:
		bool operator()(const WorldObject::Ptr& a, const WorldObject::Ptr& b) const {
			return a->getPosition().y > b->getPosition().y;
		}
	};
	sorted.sort(SortByY());
	for (ObjectList::const_iterator it = sorted.begin();
		it != sorted.end(); ++it)
	{
		(*it)->render();
	}
}

WorldRenderer::ObjectHandle WorldRenderer::addObject(
	const std::wstring& path, const Rectf& rect)
{
	m_objects.push_back(WorldObject::create(m_renderer, path, rect));
	ObjectHandle result = m_objects.end();
	--result;
	return result;
}

void WorldRenderer::setObjectPos(ObjectHandle obj, const Vector2f& pos)
{
	(*obj)->setPosition(pos);
}
