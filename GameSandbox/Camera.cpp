// Camera.cpp
// Nolan Check
// Created 1/19/2012

#include "Camera.hpp"

// Elevation angle for true isometric projection: equal to
// asin(tan(30 degrees))
// See <http://en.wikipedia.org/wiki/Isometric_projection>
static const float ISOMETRIC_ELEVATION_DEG = 35.264389682754654315377000330019f;

Camera::Camera()
{ }

Camera* Camera::Create()
{
	Camera* p(new Camera);

	p->m_center = Vector2f(0.0f, 0.0f);
	p->m_zoom = 8.0f;
	p->m_elevation = 90.0f;

	return p;
}

Matrix3x2f Camera::GetWorldToViewport(const Rectf& vp) const
{
	Rectf from;
	if ((vp.bottom - vp.top) < (vp.right - vp.left))
	{
		// Viewport is more wide than tall, so make sure m_zoom units are
		// visible on the Y axis.
		float aspect = (vp.right - vp.left) / (vp.bottom - vp.top);
		from = Rectf(
			m_center.x - m_zoom * aspect,
			m_center.y + m_zoom,
			m_center.x + m_zoom * aspect,
			m_center.y - m_zoom);
	}
	else
	{
		// Viewport is more tall than wide, so make sure m_zoom units are
		// visible on the X axis.
		float aspect = (vp.bottom - vp.top) / (vp.right - vp.left);
		from = Rectf(
			m_center.x - m_zoom,
			m_center.y + m_zoom * aspect,
			m_center.x + m_zoom,
			m_center.y - m_zoom * aspect);
	}

	// FIXME: What is the correct way to convert elevation angle to Y scaling
	// factor?
	Matrix3x2f elevationScale = Matrix3x2f::Scale(
		1.0f, sin(DegToRad(m_elevation)));

	return elevationScale * Matrix3x2f::RectLerp(from, vp);
}
