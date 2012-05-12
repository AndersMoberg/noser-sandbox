// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

#include <fstream>
#include <sstream>
#include <string>

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
	p->m_camera = Camera::create();

	// read in an xyz file
	std::ifstream ifs("bridge.xyz");
	bool boundsInited = false;
	while (!ifs.eof())
	{
		std::string line;
		std::getline(ifs, line);
		if (!line.empty())
		{
			if (line[0] == '#') {
				continue;
			}

			std::istringstream iss(line);
			float x, y, z;
			iss >> x >> y >> z;
			p->m_points.push_back(Vector3f(x, y, z));
			if (!boundsInited)
			{
				p->m_bounds.left = p->m_bounds.right = x;
				p->m_bounds.top = p->m_bounds.bottom = y;
				p->m_bounds.near = p->m_bounds.far = z;
				boundsInited = true;
			}
			else
			{
				p->m_bounds.left = std::min(x, p->m_bounds.left);
				p->m_bounds.top = std::max(y, p->m_bounds.top);
				p->m_bounds.near = std::min(z, p->m_bounds.near);
				p->m_bounds.right = std::max(x, p->m_bounds.right);
				p->m_bounds.bottom = std::min(y, p->m_bounds.bottom);
				p->m_bounds.far = std::max(z, p->m_bounds.far);
			}
		}
	}

	float zoom = (Vector3f(p->m_bounds.left, p->m_bounds.top, p->m_bounds.near) -
		Vector3f(p->m_bounds.right, p->m_bounds.bottom, p->m_bounds.far)).length();
	p->m_camera->setCenter(p->m_bounds.center());
	p->m_camera->setZoom(zoom);

	static const char DRAW_VERTEX_SHADER[] =
		"attribute vec3 a_pos;\n"
		"uniform mat4 u_mat;\n"
		"void main()\n"
		"{\n"
			"gl_Position = u_mat * vec4(a_pos, 1);\n"
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
	p->m_drawProgram.umatLoc = glGetUniformLocation(p->m_drawProgram.program, "u_mat");

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
	
	Matrix4x4f mat = m_camera->getWorldToClip(
		m_renderer->getWidth(), m_renderer->getHeight());
	glUniformMatrix4x4f(m_drawProgram.umatLoc, mat);

	for (Points::const_iterator it = m_points.begin(); it < m_points.end(); it += 2)
	{
		Vector3f points[2] = { *it, *(it+1) };
		glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), points);
		glEnableVertexAttribArray(m_drawProgram.aposLoc);
		glDrawArrays(GL_LINES, 0, 2);

		//drawSphere(*it, 0.05f, 16, 16);
	}

	m_renderer->present();
}

void Application::drawSphere(const Vector3f& center, float radius, int lats, int longs)
{
	std::vector<float> verts;

	for (int lat = 0; lat < lats; ++lat)
	{
		float theta = lat * M_PIf / lats;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon < longs; ++lon)
		{
			float phi = lon * M_2PIf / longs;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = cosPhi * sinTheta;
			float y = cosTheta;
			float z = sinPhi * sinTheta;
			verts.push_back(center.x + radius * x);
			verts.push_back(center.y + radius * y);
			verts.push_back(center.z + radius * z);
		}
	}
	
	glUseProgram(m_drawProgram.program);

	glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), &verts[0]);
	glEnableVertexAttribArray(m_drawProgram.aposLoc);

	Matrix4x4f mat = m_camera->getWorldToClip(
		m_renderer->getWidth(), m_renderer->getHeight());
	glUniformMatrix4x4f(m_drawProgram.umatLoc, mat);

	glDrawArrays(GL_POINTS, 0, lats * longs);
}
