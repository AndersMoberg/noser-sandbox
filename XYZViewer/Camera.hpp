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

	void setCenter(const Vector3f& center);
	float getZoom() const;
	void setZoom(float zoom);

	void rotateAroundX(float angle);
	void rotateAroundY(float angle);

	Matrix4x4f getWorldToClip(unsigned int width, unsigned int height) const;

private:

	Camera();

	Vector3f m_center;
	float m_zoom;
	float m_near;
	float m_far;
	Quaternionf m_rotation;

};

#endif
