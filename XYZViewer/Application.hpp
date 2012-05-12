// Application.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include <vector>

#include <GLES2/gl2.h>

#include "MainWindow.hpp"
#include "Geometry.hpp"
#include "GLES2Renderer.hpp"

class Application
{

public:

	~Application();
	static std::unique_ptr<Application> create(HINSTANCE hInstance, int nShowCmd);

	int messagePump();

	void paint();

private:

	Application();

	void drawSphere(const Vector3f& center, float radius, int lats, int longs);

	std::unique_ptr<MainWindow> m_window;
	std::unique_ptr<GLES2Renderer> m_renderer;

	typedef std::vector<Vector3f> Points;
	Points m_points;

	struct DrawProgram
	{
		DrawProgram();

		GLuint program;
		GLuint aposLoc;
		GLuint umatLoc;
	};

	DrawProgram m_drawProgram;

	Matrix4x4f m_cameraMatrix;

};

#endif
