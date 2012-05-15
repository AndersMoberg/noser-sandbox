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
	: m_unitSphereVerts(0),
	m_unitSphereIndices(0)
{ }

Application::DrawProgram::DrawProgram()
	: program(0)
{ }

Application::~Application()
{
	glDeleteProgram(m_drawProgram.program);
	m_drawProgram.program = 0;

	glDeleteBuffers(1, &m_unitSphereIndices);
	m_unitSphereIndices = 0;

	glDeleteBuffers(1, &m_unitSphereVerts);
	m_unitSphereVerts = 0;
}

static const int NUM_LATITUDES = 64;
static const int NUM_LONGITUDES = 64;

struct Vertex
{
	Vector3f pos;
	Vector3f nrm;
};

std::unique_ptr<Application> Application::create(HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<Application> p(new Application);

	p->m_window = MainWindow::create(p.get(), hInstance, nShowCmd);
	p->m_renderer = GLES2Renderer::create(p->m_window->getHWnd());

	glGenBuffers(1, &p->m_unitSphereVerts);
	glBindBuffer(GL_ARRAY_BUFFER, p->m_unitSphereVerts);
	std::vector<Vertex> unitSphereVertData;
	for (int lat = 0; lat < NUM_LATITUDES; ++lat)
	{
		float theta = lat * M_PIf / NUM_LATITUDES;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon < NUM_LONGITUDES; ++lon)
		{
			float phi = lon * M_2PIf / NUM_LONGITUDES;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = cosPhi * sinTheta;
			float y = cosTheta;
			float z = sinPhi * sinTheta;
			Vertex vert;
			vert.pos = Vector3f(x, y, z);
			vert.nrm = Vector3f(x, y, z);
			unitSphereVertData.push_back(vert);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, unitSphereVertData.size()*sizeof(Vertex), &unitSphereVertData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &p->m_unitSphereIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->m_unitSphereIndices);
	std::vector<unsigned short> unitSphereIndicesData;
	for (int lat = 0; lat < NUM_LATITUDES; ++lat)
	{
		for (int lon = 0; lon < NUM_LONGITUDES; ++lon)
		{
			unsigned short first = (lat * (NUM_LONGITUDES+1)) + lon;
			unsigned short second = first + NUM_LONGITUDES+1;
			unitSphereIndicesData.push_back(first);
			unitSphereIndicesData.push_back(second);
			unitSphereIndicesData.push_back(first+1);
			unitSphereIndicesData.push_back(second);
			unitSphereIndicesData.push_back(second+1);
			unitSphereIndicesData.push_back(first+1);
		}
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, unitSphereIndicesData.size()*sizeof(unsigned short), &unitSphereIndicesData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
		"attribute vec3 a_nrm;\n"
		"uniform mat4 u_mat;\n"
		"varying vec4 v_color;\n"
		"void main()\n"
		"{\n"
			"gl_Position = u_mat * vec4(a_pos, 1);\n"
			"v_color = vec4(a_nrm, 1);\n"
		"}\n"
		;
	static const char DRAW_FRAGMENT_SHADER[] =
		"precision mediump float;\n"
		"varying vec4 v_color;\n"
		"void main()\n"
		"{\n"
			"gl_FragColor = v_color;\n"
		"}\n"
		;
	p->m_drawProgram.program = loadGLSLProgram(DRAW_VERTEX_SHADER, DRAW_FRAGMENT_SHADER);
	p->m_drawProgram.aposLoc = glGetAttribLocation(p->m_drawProgram.program, "a_pos");
	p->m_drawProgram.anrmLoc = glGetAttribLocation(p->m_drawProgram.program, "a_nrm");
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
	m_camera->rotateAroundY(-M_PIf/32.0f);
	paint();
}

void Application::onRight()
{
	m_camera->rotateAroundY(M_PIf/32.0f);
	paint();
}

void Application::onUp()
{
	m_camera->rotateAroundX(-M_PIf/32.0f);
	paint();
}

void Application::onDown()
{
	m_camera->rotateAroundX(M_PIf/32.0f);
	paint();
}

void Application::paint()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_renderer->getWidth(), m_renderer->getHeight());

	glEnable(GL_DEPTH_TEST);

	// Draw points
	for (Points::const_iterator it = m_points.begin(); it < m_points.end(); ++it)
	{
		drawSphere(*it, 0.005f);
	}

	//// Draw lines
	//glUseProgram(m_drawProgram.program);
	//
	//Matrix4x4f mat = m_camera->getWorldToClip(
	//	m_renderer->getWidth(), m_renderer->getHeight());
	//glUniformMatrix4x4f(m_drawProgram.umatLoc, mat);

	//for (Points::const_iterator it = m_points.begin(); it < m_points.end(); it += 2)
	//{
	//	Vector3f points[2] = { *it, *(it+1) };
	//	glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), points);
	//	glEnableVertexAttribArray(m_drawProgram.aposLoc);
	//	glDrawArrays(GL_LINES, 0, 2);
	//}

	m_renderer->present();
}

void Application::drawSphere(const Vector3f& center, float radius)
{
	glUseProgram(m_drawProgram.program);

	glBindBuffer(GL_ARRAY_BUFFER, m_unitSphereVerts);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_unitSphereIndices);

	glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(m_drawProgram.aposLoc);
	glVertexAttribPointer(m_drawProgram.anrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(m_drawProgram.anrmLoc);

	Matrix4x4f mat = m_camera->getWorldToClip(
		m_renderer->getWidth(), m_renderer->getHeight());
	Matrix4x4f scale = Matrix4x4f::scale(Vector3f(radius, radius, radius));
	Matrix4x4f translate = Matrix4x4f::translate(center);
	glUniformMatrix4x4f(m_drawProgram.umatLoc, mat * translate * scale);

	glDrawElements(GL_TRIANGLES, 6*NUM_LATITUDES*NUM_LONGITUDES, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
