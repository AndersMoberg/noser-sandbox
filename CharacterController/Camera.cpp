// Camera.cpp
// Nolan Check
// Created 1/19/2012

#include "Camera.hpp"

Camera::Camera()
{ }

CameraPtr Camera::Create()
{
	CameraPtr p(new Camera);

	p->m_center = Vector2f(0.0f, 0.0f);
	p->m_zoom = 10.0f;

	return p;
}
