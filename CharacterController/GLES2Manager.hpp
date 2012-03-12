// GLES2Manager.hpp
// Nolan Check
// Created 3/12/2012

#ifndef _GLES2MANAGER_HPP
#define _GLES2MANAGER_HPP

#include <memory>

#include <EGL/egl.h>

class GLES2Manager;
typedef std::shared_ptr<GLES2Manager> GLES2ManagerPtr;

class GLES2Manager
{

public:

	~GLES2Manager();
	static GLES2ManagerPtr Create(HWND hWnd);

	void Present();

private:

	GLES2Manager();

	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;

};

#endif
