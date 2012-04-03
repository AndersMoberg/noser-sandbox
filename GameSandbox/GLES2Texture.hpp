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
	
	GLES2Texture();
	~GLES2Texture();

	GLuint get() { return m_texture; }

private:

	// Disallow copy and assign
	GLES2Texture(const GLES2Texture&);
	GLES2Texture& operator=(const GLES2Texture&);

	GLuint m_texture;

};

#endif
