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

	typedef std::shared_ptr<WorldObject> Ptr;

	static Ptr create(
		GLES2Renderer::Ptr renderer, const std::wstring& path,
		const Rectf& rect);

	void render();

	void setPosition(const Vector2f& pos);
	const Vector2f& getPosition() const { return m_pos; }

private:

	WorldObject();

	GLES2Renderer::Ptr m_renderer;
	std::unique_ptr<GLES2Texture> m_texture;
	Rectf m_rect;
	Vector2f m_pos;

};

#endif
