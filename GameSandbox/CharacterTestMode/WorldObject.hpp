// WorldObject.hpp
// Nolan Check
// Created 5/27/2012

#ifndef _WORLDOBJECT_HPP
#define _WORLDOBJECT_HPP

#include <memory>
#include <string>

#include "Geometry.hpp"
#include "GLES2Texture.hpp"
#include "GLES2Renderer.hpp"

class WorldObject
{

public:

	static std::unique_ptr<WorldObject> create(
		GLES2Renderer* renderer, const std::wstring& path,
		const Rectf& rect);

	void render();

private:

	WorldObject();

	GLES2Renderer* m_renderer;
	std::unique_ptr<GLES2Texture> m_texture;
	Rectf m_rect;

};

#endif
