// GLES2Renderer.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _GLES2RENDERER_HPP
#define _GLES2RENDERER_HPP

#include <memory>

#include <EGL/egl.h>

class GLES2Renderer
{

public:

	~GLES2Renderer();
	static std::unique_ptr<GLES2Renderer> create(HWND hWnd);

private:
	
	GLES2Renderer();

	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;

};

#endif
