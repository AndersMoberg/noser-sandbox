// Camera.hpp
// Nolan Check
// Created 1/8/2012

#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <memory>

#include "Geometry.hpp"

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Camera
{

public:

	static CameraPtr Create();

	Matrix3x2f GetCanvasToClip(const RectF& viewport) const;
	Matrix3x2f GetViewportToCanvas(const RectF& viewport) const;

private:

	Vector2f m_center;
	float m_zoom; // Pixels per canvas unit

};

#endif
