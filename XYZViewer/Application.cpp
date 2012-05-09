// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

#include <GLES2/gl2.h>

#include "GLES2Utils.hpp"

Application::Application()
{ }

Application::DrawProgram::DrawProgram()
	: program(0)
{ }

Application::~Application()
{
	glDeleteProgram(m_drawProgram.program);
	m_drawProgram.program = 0;
}

std::unique_ptr<Application> Application::create(HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<Application> p(new Application);

	p->m_window = MainWindow::create(p.get(), hInstance, nShowCmd);
	p->m_renderer = GLES2Renderer::create(p->m_window->getHWnd());

	p->m_points.push_back(Vector3f(0.0f, 0.5f, 0.0f));
	p->m_points.push_back(Vector3f(0.5f, 0.0f, 0.0f));

	static const char DRAW_VERTEX_SHADER[] =
		"attribute vec3 a_pos;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vec4(a_pos, 1);\n"
		"}\n"
		;
	static const char DRAW_FRAGMENT_SHADER[] =
		"precision mediump float;\n"
		"void main()\n"
		"{\n"
			"gl_FragColor = vec4(0, 0, 0, 1);\n"
		"}\n"
		;
	p->m_drawProgram.program = loadGLSLProgram(DRAW_VERTEX_SHADER, DRAW_FRAGMENT_SHADER);
	p->m_drawProgram.aposLoc = glGetAttribLocation(p->m_drawProgram.program, "a_pos");

	return p;
}

int Application::messagePump()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void Application::paint()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_renderer->getWidth(), m_renderer->getHeight());

	glUseProgram(m_drawProgram.program);

	for (Points::const_iterator it = m_points.begin(); it != m_points.end(); ++it)
	{
		glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), &*it);
		glEnableVertexAttribArray(m_drawProgram.aposLoc);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	m_renderer->present();
}
