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

	glDeleteBuffers(1, &m_unitCylinderVerts);
	m_unitCylinderVerts = 0;

	glDeleteBuffers(1, &m_unitSphereIndices);
	m_unitSphereIndices = 0;

	glDeleteBuffers(1, &m_unitSphereVerts);
	m_unitSphereVerts = 0;
}

static const int NUM_LATITUDES = 32;
static const int NUM_LONGITUDES = 32;
static const int NUM_CYLINDER_DIVS = 32;

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
		float theta = lat * M_PIf / (NUM_LATITUDES-1);
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon < NUM_LONGITUDES; ++lon)
		{
			float phi = lon * M_2PIf / (NUM_LONGITUDES-1);
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

	glGenBuffers(1, &p->m_unitCylinderVerts);
	glBindBuffer(GL_ARRAY_BUFFER, p->m_unitCylinderVerts);
	std::vector<Vertex> unitCylinderVertData;
	for (int i = 0; i < NUM_CYLINDER_DIVS; ++i)
	{
		float theta = i * M_2PIf / (NUM_CYLINDER_DIVS-1);
		float x = cos(theta);
		float y = sin(theta);
		Vertex va;
		va.pos = Vector3f(x, y, 0.0f);
		va.nrm = Vector3f(x, y, 0.0f);
		Vertex vb;
		vb.pos = Vector3f(x, y, 1.0f);
		vb.nrm = Vector3f(x, y, 0.0f);

		unitCylinderVertData.push_back(va);
		unitCylinderVertData.push_back(vb);
	}
	glBufferData(GL_ARRAY_BUFFER, unitCylinderVertData.size()*sizeof(Vertex), &unitCylinderVertData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	p->m_camera = Camera::create();

	p->m_model = Model::loadFromFile(L"helix.xyz");

	const Boxf& bounds = p->m_model->getBounds();
	float zoom = (Vector3f(bounds.left, bounds.top, bounds.near) -
		Vector3f(bounds.right, bounds.bottom, bounds.far)).length();
	p->m_camera->setCenter(bounds.center());
	p->m_camera->setZoom(zoom);

	static const char DRAW_VERTEX_SHADER[] =
		"attribute vec3 a_pos;\n"
		"attribute vec3 a_nrm;\n"
		"uniform mat4 u_mat;\n"
		"varying vec4 v_color;\n"
		"void main()\n"
		"{\n"
			"gl_Position = u_mat * vec4(a_pos, 1);\n"
			"vec3 normal = normalize(vec3(u_mat * vec4(a_nrm, 0)));\n"
			"vec3 lightDir = normalize(vec3(0, 1, -1));\n"
			"float d = dot(normal, lightDir);\n"
			"v_color = vec4(d, d, d, 1);\n"
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
	glClearColor(0.8f, 0.2f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_renderer->getWidth(), m_renderer->getHeight());

	glEnable(GL_DEPTH_TEST);

	// Draw points
	const Model::Points& points = m_model->getPoints();
	for (Model::Points::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		drawSphere(*it, 0.02f);
	}

	// Draw lines
	const Model::Lines& lines = m_model->getLines();
	for (Model::Lines::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		drawCylinder(points[it->first], points[it->second], 0.01f);
	}

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

void Application::drawCylinder(const Vector3f& a, const Vector3f& b, float radius)
{
	glUseProgram(m_drawProgram.program);

	Vector3f perp1 = (b-a).arbitraryPerpendicular();
	perp1.normalize();
	Vector3f perp2 = Vector3f::cross(perp1, b-a);
	perp2.normalize();
	
	std::vector<Vertex> unitCylinderVertData;
	for (int i = 0; i < NUM_CYLINDER_DIVS; ++i)
	{
		float theta = i * M_2PIf / (NUM_CYLINDER_DIVS-1);
		float x = cos(theta);
		float y = sin(theta);
		Vertex va;
		va.pos = a + radius*x*perp1 + radius*y*perp2;
		va.nrm = x*perp1 + y*perp2;
		Vertex vb;
		vb.pos = b + radius*x*perp1 + radius*y*perp2;
		vb.nrm = x*perp1 + y*perp2;

		unitCylinderVertData.push_back(va);
		unitCylinderVertData.push_back(vb);
	}
	glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &unitCylinderVertData[0].pos);
	glEnableVertexAttribArray(m_drawProgram.aposLoc);
	glVertexAttribPointer(m_drawProgram.anrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &unitCylinderVertData[0].nrm);
	glEnableVertexAttribArray(m_drawProgram.anrmLoc);

	// TODO: Switch to vertex buffer method when math has been worked out
	//glBindBuffer(GL_ARRAY_BUFFER, m_unitCylinderVerts);
	//
	//glVertexAttribPointer(m_drawProgram.aposLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//glEnableVertexAttribArray(m_drawProgram.aposLoc);
	//glVertexAttribPointer(m_drawProgram.anrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	//glEnableVertexAttribArray(m_drawProgram.anrmLoc);
	//

	Matrix4x4f mat = m_camera->getWorldToClip(
		m_renderer->getWidth(), m_renderer->getHeight());
	glUniformMatrix4x4f(m_drawProgram.umatLoc, mat);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2*NUM_CYLINDER_DIVS);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}
