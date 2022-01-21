#include "camera.hpp"
#include "frustum.hpp"

Camera::Camera(glm::vec3 position, float fov, int width, int height) : _fov(fov), _position(position), _width(width), _height(height), _frustum(new Frustum())
{
	_yaw = 90.0;
	_pitch = 0.0;
	_lastX = height / 2.0;
	_lastY = width / 2.0;
	_front = glm::vec3(0.0f, 0.0f, 1.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

//direction
void Camera::updateDirection()
{
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));

	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front = glm::normalize(front);

	_right = glm::normalize(glm::cross(_front, {0, 1, 0}));
	_up = glm::normalize(glm::cross(_right, _front));
}

float Camera::offsetPitch(float _offset, float _sensitivity)
{
	_pitch += _offset * _sensitivity;
	if (_pitch >= 85)
	{
		_pitch = 85.0f;
	}
	if (_pitch <= -85)
	{
		_pitch = -85.0f;
	}

	updateDirection();
	return _pitch;
}

float Camera::offsetYaw(float _offset, float _sensitivity)
{
	_yaw += _offset * _sensitivity;
	if (_yaw < 0)
		_yaw += 360;
	if (_yaw > 360)
		_yaw -= 360;
	updateDirection();
	return _yaw;
}

//_fov
float Camera::offsetFov(float _offset)
{
	if (_fov + _offset > 90)
	{
		_fov = 90.0f;
	}
	else if (_fov + _offset < 1)
	{
		_fov = 1.0f;
	}
	_fov += _offset;
	return _fov;
}

//position
void Camera::moveX(float _offset)
{
	_position += glm::normalize(glm::cross(_front, _up)) * _offset;
}

void Camera::moveY(float _offset)
{
	_position += glm::normalize(_up) * _offset;
}

void Camera::moveZ(float _offset)
{
	_position += _front * _offset;
}

void Camera::setRes(int w, int h)
{
	_width = w;
	_height = h;
	_lastX = h / 2.0;
	_lastY = w / 2.0;
}

void Camera::move(const glm::vec3 delta)
{
	//TODO
}

void Camera::rotate(const glm::vec3 delta)
{
	//TODO
}