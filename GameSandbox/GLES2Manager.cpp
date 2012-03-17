// GLES2Manager.cpp
// Nolan Check
// Created 3/16/2012

#include "GLES2Manager.hpp"

GLES2Manager::GLES2Manager()
	: m_eglDisplay(0),
	m_eglSurface(0),
	m_eglContext(0)
{ }

GLES2Manager::TexturedQuadProgram::TexturedQuadProgram()
	: program(0)
{ }

GLES2Manager::~GLES2Manager()
{
	glDeleteProgram(m_texturedQuadProgram.program);
	m_texturedQuadProgram.program = 0;

	eglDestroyContext(m_eglDisplay, m_eglContext);
	m_eglContext = 0;

	eglDestroySurface(m_eglDisplay, m_eglSurface);
	m_eglSurface = 0;

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

void GLES2Manager::DrawTexturedQuad(const Rectf& rc)
{
	glUseProgram(m_texturedQuadProgram.program);

	GLfloat verts[] = { rc.left, rc.bottom, 0.0f, 1.0f, // Lower left
		rc.right, rc.bottom, 1.0f, 1.0f, // Lower right
		rc.left, rc.top, 0.0f, 0.0f, // Upper left
		rc.right, rc.top, 1.0f, 0.0f }; // Upper right

	glVertexAttribPointer(m_texturedQuadProgram.aposLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[0]);
	glVertexAttribPointer(m_texturedQuadProgram.atexLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[2]);

	glEnableVertexAttribArray(m_texturedQuadProgram.aposLoc);
	glEnableVertexAttribArray(m_texturedQuadProgram.atexLoc);

	glUniform1i(m_texturedQuadProgram.usamplerLoc, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
