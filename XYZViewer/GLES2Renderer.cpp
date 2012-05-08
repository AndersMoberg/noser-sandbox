// GLES2Renderer.cpp
// Nolan Check
// Created 5/7/2012

#include "GLES2Renderer.hpp"

GLES2Renderer::GLES2Renderer()
	: m_eglDisplay(0),
	m_eglSurface(0),
	m_eglContext(0)
{ }

GLES2Renderer::~GLES2Renderer()
{
	eglDestroyContext(m_eglDisplay, m_eglContext);
	m_eglContext = 0;

	eglDestroySurface(m_eglDisplay, m_eglSurface);
	m_eglSurface = 0;

	eglTerminate(m_eglDisplay);
	m_eglDisplay = 0;
}

bool GLES2Renderer::init(HWND hWnd)
{
	m_eglDisplay = eglGetDisplay(GetDC(hWnd));
	if (m_eglDisplay == EGL_NO_DISPLAY) {
		return false;
	}

	if (eglInitialize(m_eglDisplay, NULL, NULL) != EGL_TRUE) {
		return false;
	}
	
	EGLint numConfigs;
	if (eglGetConfigs(m_eglDisplay, NULL, 0, &numConfigs) != EGL_TRUE) {
		return false;
	}

	static const EGLint chooseConfigAttribs[] = {
		EGL_NONE
	};
	EGLConfig eglConfig;
	if (eglChooseConfig(m_eglDisplay, chooseConfigAttribs, &eglConfig, 1, &numConfigs) != EGL_TRUE) {
		return false;
	}

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, hWnd, NULL);
	if (m_eglSurface == EGL_NO_SURFACE) {
		return false;
	}

	static const EGLint createContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	m_eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, createContextAttribs);
	if (m_eglContext == EGL_NO_CONTEXT) {
		return false;
	}

	if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) != EGL_TRUE) {
		return false;
	}

	return true;
}
