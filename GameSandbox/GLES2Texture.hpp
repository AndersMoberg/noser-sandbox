// GLES2Texture.hpp
// Nolan Check
// Created 3/14/2012

#ifndef _GLES2TEXTURE_HPP
#define _GLES2TEXTURE_HPP

#include <memory>

#include <GLES2/gl2.h>

class GLES2Texture
{

public:

	~GLES2Texture();
	static GLES2Texture* Create();

	GLuint Get() { return m_texture; }

private:

	GLES2Texture();

	GLuint m_texture;

};

#endif
