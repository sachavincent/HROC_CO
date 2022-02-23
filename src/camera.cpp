#include "camera.hpp"
#include "frustum.hpp"

#include "bvh/boundingBoxObject.hpp"
#include "scene.hpp"

Camera::Camera(int _width, int _height, glm::vec3 _position, float _fov) : width(_width), height(_height), position(_position), fov(_fov), movingFactor({0, 0, 0})
{
	yaw = 90.0;
	pitch = 0.0;
	front = glm::vec3(0.0f, 0.0f, 1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	nearDistance = 1.0f;
	moveSpeed = 4.0;

	farDistance = 500.0f;

	setResolution(_width, _height);
	frustum = new Frustum(this);
}

// direction
void Camera::updateDirection()
{
	glm::vec3 _front;
	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(_front);

	right = glm::normalize(glm::cross(front, {0, 1, 0}));
	up = glm::normalize(glm::cross(right, front));

	frustum->update(this);
}

float Camera::offsetPitch(float _offset, float _sensitivity)
{
	pitch += _offset * _sensitivity;
	if (pitch >= 85)
	{
		pitch = 85.0f;
	}
	if (pitch <= -85)
	{
		pitch = -85.0f;
	}

	updateDirection();
	return pitch;
}

float Camera::offsetYaw(float _offset, float _sensitivity)
{
	yaw += _offset * _sensitivity;
	if (yaw < 0)
		yaw += 360;
	if (yaw > 360)
		yaw -= 360;

	updateDirection();
	return yaw;
}

//_fov
float Camera::offsetFov(float _offset)
{
	if (fov + _offset > 180)
	{
		fov = 180.0f;
	}
	else if (fov + _offset < 1)
	{
		fov = 1.0f;
	}

	fov += _offset;
	return fov;
}

// position
void Camera::moveX(float _offset)
{
	movingFactor = movingFactor + glm::vec3(_offset, 0, 0);
}

void Camera::moveY(float _offset)
{
	movingFactor = movingFactor + glm::vec3(0, _offset, 0);
}

void Camera::moveZ(float _offset)
{
	movingFactor = movingFactor + glm::vec3(0, 0, _offset);
}

void Camera::setResolution(int w, int h)
{
	width = w;
	height = h;
	lastX = (float)w / 2.0;
	lastY = (float)h / 2.0;
}

void Camera::move(const float delta)
{
	float d = delta;
	if(constantSpeed) 
		d = 0.05;
	position += movingFactor.x * moveSpeed * d * glm::normalize(glm::cross(front, up));
	position += movingFactor.y * moveSpeed * d * glm::normalize(up);
	position += movingFactor.z * moveSpeed * d * front;

	frustum->update(this);
}

void Camera::setCameraInfo(CameraInfo _cameraInfo)
{
	if (_cameraInfo.position)
		position = _cameraInfo.position.value();
	if (_cameraInfo.fov)
		fov = _cameraInfo.fov.value();
	if (_cameraInfo.yaw)
		yaw = _cameraInfo.yaw.value();
	if (_cameraInfo.pitch)
		pitch = _cameraInfo.pitch.value();
}