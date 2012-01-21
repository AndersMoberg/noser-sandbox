// Camera.hpp
// Nolan Check
// Created 1/19/2012

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

	Matrix3x2f GetWorldToViewport(const Rectf& vp) const;

private:

	Camera();

	Vector2f m_center;
	float m_zoom; // Number of world units visible on shortest axis

};

#endif
