// Camera.cpp
// Nolan Check
// Created 5/11/2012

#include "Camera.hpp"

Camera::Camera()
{ }

std::unique_ptr<Camera> Camera::create()
{
	std::unique_ptr<Camera> p(new Camera);

	p->m_zoom = 0.25f;
	p->m_near = -1.0f;
	p->m_far = 1.0f;
	p->m_xRotate = M_PIf / 2.0f;
	p->m_yRotate = M_PIf / 4.0f;

	return p;
}

Matrix4x4f Camera::getWorldToClip(unsigned int width, unsigned int height) const
{
	Boxf from;
	if (height < width)
	{
		// Viewport is more wide than tall, so make sure m_zoom units are
		// visible on the Y axis.
		float aspect = (float)width / height;
		from = Boxf(
			-m_zoom * aspect,
			m_zoom,
			m_near,
			m_zoom * aspect,
			-m_zoom,
			m_far);
	}
	else
	{
		// Viewport is more tall than wide, so make sure m_zoom units are
		// visible on the X axis.
		float aspect = (float)height / width;
		from = Boxf(
			-m_zoom,
			m_zoom * aspect,
			m_near,
			m_zoom,
			-m_zoom * aspect,
			m_far);
	}

	Boxf to(-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f);

	Matrix4x4f xRotate = Matrix4x4f::rotateX(m_xRotate);
	Matrix4x4f yRotate = Matrix4x4f::rotateY(m_yRotate);

	return Matrix4x4f::boxLerp(from, to) * xRotate * yRotate;
}
