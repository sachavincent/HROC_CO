#include "camera.hpp"
#include "frustum.hpp"

Camera::Camera(int width, int height, glm::vec3 position, float fov) : _width(width), _height(height), _position(position), _fov(fov), movingFactor({0, 0, 0})
{
	_yaw = 90.0;
	_pitch = 0.0;
	_front = glm::vec3(0.0f, 0.0f, 1.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_nearDistance = 1.0f;
	_farDistance = 200.0f;
	
	setResolution(width, height);

	_frustum = new Frustum();
}

// direction
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
	if (_fov + _offset > 180)
	{
		_fov = 180.0f;
	}
	else if (_fov + _offset < 1)
	{
		_fov = 1.0f;
	}
	_fov += _offset;
	return _fov;
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
	_width = w;
	_height = h;
	_lastX = (float)w / 2.0;
	_lastY = (float)h / 2.0;
}

void Camera::move(const float delta)
{
	_position += movingFactor.x * delta * glm::normalize(glm::cross(_front, _up));
	_position += movingFactor.y * delta * glm::normalize(_up);
	_position += movingFactor.z * delta * _front;
}

void Camera::rotate(const glm::vec3 delta)
{
	// TODO
}