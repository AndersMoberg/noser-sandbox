// GLES2Manager.hpp
// Nolan Check
// Created 3/16/2012

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

	void SetTexturedQuadMatrix(const Matrix3x2f& mat);
	void DrawTexturedQuad(const Rectf& rc);
	
	void Present();
	void Resize();

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

private:

	GLES2Manager();

	HWND m_hWnd;

	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;

	unsigned int m_width;
	unsigned int m_height;
	
	struct TexturedQuadProgram
	{
		TexturedQuadProgram();

		GLuint program;
		GLuint aposLoc;
		GLuint atexLoc;
		GLuint umatLoc;
		GLuint uaddLoc;
		GLuint usamplerLoc;
	} m_texturedQuadProgram;

};

#endif
