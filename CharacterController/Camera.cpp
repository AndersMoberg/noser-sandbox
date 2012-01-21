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

Matrix3x2f Camera::GetWorldToViewport(const Rectf& vp) const
{
	// TODO: Correct for aspect ratio
	Rectf visibleRc(
		m_center.x - m_zoom / 2.0f,
		m_center.y + m_zoom / 2.0f,
		m_center.x + m_zoom / 2.0f,
		m_center.y - m_zoom / 2.0f);
	return Matrix3x2f::RectLerp(visibleRc, vp);
}
