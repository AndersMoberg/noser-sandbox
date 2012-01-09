// Camera.cpp
// Nolan Check
// Created 1/8/2012

#include "Camera.hpp"

CameraPtr Camera::Create()
{
	CameraPtr p(new Camera);

	p->m_center = Vector2f(0.0f, 0.0f);
	p->m_zoom = 96.0f;

	return p;
}

Matrix3x2f Camera::GetCanvasToClip(const RectF& viewport) const
{
	float xVisible = (viewport.right - viewport.left) / m_zoom;
	float yVisible = (viewport.bottom - viewport.top) / m_zoom;
	RectF visibleRc(
		m_center.x - xVisible / 2.0f,
		m_center.y + yVisible / 2.0f,
		m_center.x + xVisible / 2.0f,
		m_center.y - yVisible / 2.0f);
	return Matrix3x2f::RectLerp(visibleRc, RectF(-1.0f, 1.0f, 1.0f, -1.0f));
}

Matrix3x2f Camera::GetViewportToCanvas(const RectF& viewport) const
{
	float xVisible = (viewport.right - viewport.left) / m_zoom;
	float yVisible = (viewport.bottom - viewport.top) / m_zoom;
	RectF visibleRc(
		m_center.x - xVisible / 2.0f,
		m_center.y + yVisible / 2.0f,
		m_center.x + xVisible / 2.0f,
		m_center.y - yVisible / 2.0f);
	return Matrix3x2f::RectLerp(viewport, visibleRc);
}
