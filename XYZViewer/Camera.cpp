// Camera.cpp
// Nolan Check
// Created 5/11/2012

#include "Camera.hpp"

Camera::Camera()
{ }

std::unique_ptr<Camera> Camera::create()
{
	std::unique_ptr<Camera> p(new Camera);

	p->m_center = Vector3f(0.0f, 0.0f, 0.0f);
	p->m_zoom = 0.25f;
	p->m_near = -64.0f;
	p->m_far = 64.0f;
	p->m_xRotation = 0.0f;
	p->m_yRotation = 0.0f;

	return p;
}

void Camera::setCenter(const Vector3f& center)
{
	m_center = center;
}

void Camera::setZoom(float zoom)
{
	m_zoom = zoom;
}

float Camera::getXRotation() const
{
	return m_xRotation;
}

void Camera::setXRotation(float xRotation)
{
	m_xRotation = xRotation;
}

float Camera::getYRotation() const
{
	return m_yRotation;
}

void Camera::setYRotation(float yRotation)
{
	m_yRotation = yRotation;
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
			-0.5f*m_zoom * aspect,
			0.5f*m_zoom,
			m_near,
			0.5f*m_zoom * aspect,
			-0.5f*m_zoom,
			m_far);
	}
	else
	{
		// Viewport is more tall than wide, so make sure m_zoom units are
		// visible on the X axis.
		float aspect = (float)height / width;
		from = Boxf(
			-0.5f*m_zoom,
			0.5f*m_zoom * aspect,
			m_near,
			0.5f*m_zoom,
			-0.5f*m_zoom * aspect,
			m_far);
	}

	Boxf to(-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f);

	Matrix4x4f translate = Matrix4x4f::translate(-m_center);
	Matrix4x4f xRotate = Matrix4x4f::rotateX(m_xRotation);
	Matrix4x4f yRotate = Matrix4x4f::rotateY(m_yRotation);

	return Matrix4x4f::boxLerp(from, to) * yRotate * xRotate * translate;
}
