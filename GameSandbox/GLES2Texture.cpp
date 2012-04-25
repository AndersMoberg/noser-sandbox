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

void GLES2Texture::init()
{
	glGenTextures(1, &m_texture);
}

GLuint GLES2Texture::get()
{
	assert(m_texture != 0);
	return m_texture;
}
