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

std::unique_ptr<GLES2Renderer> GLES2Renderer::create(HWND hWnd)
{
	std::unique_ptr<GLES2Renderer> p(new GLES2Renderer);

	RECT clientRc;
	GetClientRect(hWnd, &clientRc);
	p->m_width = clientRc.right - clientRc.left;
	p->m_height = clientRc.bottom - clientRc.top;

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
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
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

void GLES2Renderer::present()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

unsigned int GLES2Renderer::getWidth() const
{
	EGLint result;
	eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_WIDTH, &result);
	return result;
}

unsigned int GLES2Renderer::getHeight() const
{
	EGLint result;
	eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_HEIGHT, &result);
	return result;
}
