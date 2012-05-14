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
	: m_unitSphereVerts(0)
{ }

Application::DrawProgram::DrawProgram()
	: program(0)
{ }

Application::~Application()
{
	glDeleteProgram(m_drawProgram.program);
	m_drawProgram.program = 0;

	glDeleteBuffers(1, &m_unitSphereVerts);
	m_unitSphereVerts = 0;
}

std::unique_ptr<Application> Application::create(HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<Application> p(new Application);

	p->m_window = MainWindow::create(p.get(), hInstance, nShowCmd);
	p->m_renderer = GLES2Renderer::create(p->m_window->getHWnd());

	glGenBuffers(1, &p->m_unitSphereVerts);
	glBindBuffer(GL_ARRAY_BUFFER, p->m_unitSphereVerts);
	std::vector<float> unitSphereVertData;
	for (int lat = 0; lat < 64; ++lat)
	{
		float theta = lat * M_PIf / 64.0f;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon < 64; ++lon)
		{
			float phi = lon * M_2PIf / 64.0f;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = cosPhi * sinTheta;
			float y = cosTheta;
			float z = sinPhi * sinTheta;
			unitSphereVertData.push_back(x);
			unitSphereVertData.push_back(y);
			unitSphereVertData.push_back(z);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, unitSphereVertData.size()*sizeof(float), &unitSphereVertData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	p->m_camera = Camera::create();

	// read in an xyz file
	std::ifstream ifs("bridge.xyz");
	bool boundsInited = false;
	std::string line;
	while (ifs)
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

void Application::onLeft()
{
	//m_camera->setYRotation(m_camera->getYRotation() - M_PIf/32.0f);
	m_camera->rotateAroundY(-M_PIf/32.0f);
	paint();
}

void Application::onRight()
{
	//m_camera->setYRotation(m_camera->getYRotation() + M_PIf/32.0f);
	m_camera->rotateAroundY(M_PIf/32.0f);
	paint();
}

void Application::onUp()
{
	//m_camera->setXRotation(m_camera->getXRotation() - M_PIf/32.0f);
	m_camera->rotateAroundX(-M_PIf/32.0f);
	paint();
}

void Application::onDown()
{
	//m_camera->setXRotation(m_camera->getXRotation() + M_PIf/32.0f);
	m_camera->rotateAroundX(M_PIf/32.0f);
	paint();
}

void Application::paint()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_renderer->getWidth(), m_renderer->getHeight());
	
	// Draw points
	//for (Points::const_iterator it = m_points.begin(); it < m_points.end(); ++it)
	//{
	//	drawSphere(*it, 0.0005f);
	//}

	// Draw lines
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
	}

	m_renderer->present();
}

void Application::drawSphere(const Vector3f& center, float radius)
{
	glUseProgram(m_drawProgram.program);

	glBindBuffer(GL_ARRAY_BUFFER, m_unitSphereVerts);

	glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(m_drawProgram.aposLoc);

	Matrix4x4f mat = m_camera->getWorldToClip(
		m_renderer->getWidth(), m_renderer->getHeight());
	Matrix4x4f scale = Matrix4x4f::scale(Vector3f(radius, radius, radius));
	Matrix4x4f translate = Matrix4x4f::translate(center);
	glUniformMatrix4x4f(m_drawProgram.umatLoc, mat * translate * scale);

	glDrawArrays(GL_POINTS, 0, 64*64);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
