// GLES2Texture.cpp
// Nolan Check
// Created 3/14/2012

#include "GLES2Texture.hpp"

#include <cassert>

GLES2Texture::GLES2Texture()
	: m_texture(0)
{ }

GLES2Texture::~GLES2Texture()
{
	glDeleteTextures(1, &m_texture);
	m_texture = 0;
}

std::unique_ptr<GLES2Texture> GLES2Texture::create()
{
	std::unique_ptr<GLES2Texture> p(new GLES2Texture);

	glGenTextures(1, &p->m_texture);

	return p;
}

GLuint GLES2Texture::get()
{
	assert(m_texture != 0);
	return m_texture;
}
