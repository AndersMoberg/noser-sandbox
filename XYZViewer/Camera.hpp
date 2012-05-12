// Camera.hpp
// Nolan Check
// Created 5/11/2012

#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <memory>

#include "Geometry.hpp"

class Camera
{

public:

	static std::unique_ptr<Camera> create();

	Matrix4x4f getWorldToClip(unsigned int width, unsigned int height) const;

private:

	Camera();

	float m_zoom;
	float m_near;
	float m_far;
	float m_xRotate;
	float m_yRotate;

};

#endif
