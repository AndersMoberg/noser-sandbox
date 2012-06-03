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

	typedef std::list<WorldObject::Ptr> ObjectList;
	typedef ObjectList::const_iterator ObjectHandle;

	ObjectHandle addObject(const std::wstring& path, const Rectf& rect);
	void setObjectPos(ObjectHandle obj, const Vector2f& pos);

private:

	WorldRenderer();

	GLES2Renderer* m_renderer;
	ObjectList m_objects;

};

#endif
