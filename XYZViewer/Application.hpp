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
#include "Camera.hpp"
#include "Model.hpp"

class Application
{

public:

	~Application();
	static std::unique_ptr<Application> create(HINSTANCE hInstance, LPCTSTR lpCmdLine, int nShowCmd);

	int messagePump();

	void onLeft();
	void onRight();
	void onUp();
	void onDown();
	void paint();

private:

	Application();

	void drawSphere(const Vector3f& center, float radius);
	void drawCylinder(const Vector3f& a, const Vector3f& b, float radius);

	std::unique_ptr<MainWindow> m_window;
	std::unique_ptr<GLES2Renderer> m_renderer;

	GLuint m_unitSphereVerts;
	GLuint m_unitSphereIndices;
	GLuint m_unitCylinderVerts;

	std::unique_ptr<Model> m_model;

	struct DrawProgram
	{
		DrawProgram();

		GLuint program;
		GLint aposLoc;
		GLint anrmLoc;
		GLint umatLoc;
	};

	DrawProgram m_drawProgram;

	std::unique_ptr<Camera> m_camera;

};

#endif
