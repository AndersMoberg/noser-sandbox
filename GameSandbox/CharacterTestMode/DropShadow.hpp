// DropShadow.hpp
// Nolan Check
// Created 4/4/2012

#ifndef	_DROPSHADOW_HPP
#define _DROPSHADOW_HPP

#include <memory>

#include <GLES2/gl2.h>

#include "Geometry.hpp"

namespace CharacterTestMode
{

class DropShadowCommon
{
	friend class DropShadow;

public:

	static std::unique_ptr<DropShadowCommon> create();
	~DropShadowCommon();

private:

	DropShadowCommon();

	GLuint m_program;
	GLuint m_aposLoc;
	GLuint m_atexLoc;
	GLuint m_uoffsetLoc;
	GLuint m_usampleOffsetLoc;
	GLuint m_usamplerLoc;

};

class DropShadow
{

public:

	static std::unique_ptr<DropShadow>
		create(DropShadowCommon* common, GLuint srcTexture, int width, int height);
	~DropShadow();

	void generate(const Vector2f& offset, const Vector2f& blurSize);
	GLuint getTexture() { return m_dstTexture; }

private:

	DropShadow();

	DropShadowCommon* m_common;
	GLuint m_srcTexture;
	int m_width;
	int m_height;
	
	GLuint m_tempFramebuffer;
	GLuint m_tempTexture;
	GLuint m_dstFramebuffer;
	GLuint m_dstTexture;

};

}

#endif
