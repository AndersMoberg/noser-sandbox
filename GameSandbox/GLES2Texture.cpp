// GLES2Texture.cpp
// Nolan Check
// Created 3/14/2012

#include "GLES2Texture.hpp"

GLES2Texture::GLES2Texture()
	: m_texture(0)
{
	glGenTextures(1, &m_texture);
}

GLES2Texture::~GLES2Texture()
{
	glDeleteTextures(1, &m_texture);
	m_texture = 0;
}
