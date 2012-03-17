// GLES2Texture.hpp
// Nolan Check
// Created 3/14/2012

#ifndef _GLES2TEXTURE_HPP
#define _GLES2TEXTURE_HPP

#include <memory>

#include <GLES2/gl2.h>

class GLES2Texture;
typedef std::shared_ptr<GLES2Texture> GLES2TexturePtr;

class GLES2Texture
{

public:

	~GLES2Texture();
	static GLES2TexturePtr Create();

	GLuint Get() { return m_texture; }

private:

	GLES2Texture();

	GLuint m_texture;

};

#endif
