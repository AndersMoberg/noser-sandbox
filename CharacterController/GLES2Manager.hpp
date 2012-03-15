// GLES2Manager.hpp
// Nolan Check
// Created 3/12/2012

#ifndef _GLES2MANAGER_HPP
#define _GLES2MANAGER_HPP

#include <memory>
#include <string>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "Geometry.hpp"
#include "GLES2Texture.hpp"

class GLES2Manager;
typedef std::shared_ptr<GLES2Manager> GLES2ManagerPtr;

class GLES2Manager
{

public:

	~GLES2Manager();
	static GLES2ManagerPtr Create(HWND hWnd);

	GLES2TexturePtr CreateTextureFromFile(const std::wstring& path);

	void DrawTexturedQuad(const Rectf& rc);
	void Present();

private:

	GLES2Manager();

	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;

	struct TexturedQuadProgram
	{
		TexturedQuadProgram();

		GLuint program;
		GLuint aposLoc;
		GLuint atexLoc;
		GLuint usamplerLoc;
	} m_texturedQuadProgram;

};

#endif
