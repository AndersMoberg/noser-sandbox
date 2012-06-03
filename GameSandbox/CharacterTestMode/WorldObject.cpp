// WorldObject.cpp
// Nolan Check
// Created 5/29/2012

#include "WorldObject.hpp"

WorldObject::WorldObject()
	: m_pos(0.0f, 0.0f)
{ }

WorldObject::Ptr WorldObject::create(
	GLES2Renderer* renderer, const std::wstring& path, const Rectf& rect)
{
	WorldObject::Ptr p(new WorldObject);

	p->m_renderer = renderer;
	p->m_texture = renderer->createTextureFromFile(path);
	p->m_rect = rect;

	return p;
}

void WorldObject::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->DrawTexturedQuad(m_rect.Offset(m_pos));
}

void WorldObject::setPosition(const Vector2f& pos)
{
	m_pos = pos;
}
