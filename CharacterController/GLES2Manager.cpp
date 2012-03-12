// GLES2Manager.cpp
// Nolan Check
// Created 3/12/2012

#include "GLES2Manager.hpp"

GLES2Manager::GLES2Manager()
	: m_eglDisplay(0),
	m_eglSurface(0),
	m_eglContext(0)
{ }

GLES2Manager::~GLES2Manager()
{
	eglTerminate(m_eglDisplay);
	m_eglDisplay = 0;
}

GLES2ManagerPtr GLES2Manager::Create(HWND hWnd)
{
	GLES2ManagerPtr p(new GLES2Manager);

	p->m_eglDisplay = eglGetDisplay(GetDC(hWnd));
	if (p->m_eglDisplay == EGL_NO_DISPLAY) {
		throw std::exception("Failed to get display for EGL");
	}

	if (eglInitialize(p->m_eglDisplay, NULL, NULL) != EGL_TRUE) {
		throw std::exception("Failed to initialize EGL");
	}

	EGLint numConfigs;
	if (eglGetConfigs(p->m_eglDisplay, NULL, 0, &numConfigs) != EGL_TRUE) {
		throw std::exception("Failed to get number of EGL configurations");
	}

	static const EGLint chooseConfigAttribs[] = {
		EGL_NONE
	};
	EGLConfig eglConfig;
	if (eglChooseConfig(p->m_eglDisplay, chooseConfigAttribs, &eglConfig, 1, &numConfigs) != EGL_TRUE) {
		throw std::exception("Failed to choose an EGL configuration");
	}

	p->m_eglSurface = eglCreateWindowSurface(p->m_eglDisplay, eglConfig, hWnd, NULL);
	if (p->m_eglSurface == EGL_NO_SURFACE) {
		throw std::exception("Failed to create EGL window surface");
	}

	static const EGLint createContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	p->m_eglContext = eglCreateContext(p->m_eglDisplay, eglConfig, EGL_NO_CONTEXT, createContextAttribs);
	if (p->m_eglContext == EGL_NO_CONTEXT) {
		throw std::exception("Failed to create EGL context");
	}

	if (eglMakeCurrent(p->m_eglDisplay, p->m_eglSurface, p->m_eglSurface, p->m_eglContext) != EGL_TRUE) {
		throw std::exception("Failed to make EGL context current");
	}

	return p;
}

void GLES2Manager::Present()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}
