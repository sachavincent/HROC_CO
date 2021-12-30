#include "camera.hpp"

Camera::Camera(glm::vec3 _cameraPos, double _fov, int _resWidth, int _resHeight) :
fov(_fov),
cameraPos(_cameraPos)
{
	yaw = 90.0;
	pitch = 0.0;
	lastX = _resHeight / 2.0;
	lastY = _resWidth / 2.0;
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


	resWidth = _resWidth;
	resHeight = _resHeight;
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::vec3 Camera::getDir() {
	return cameraFront;
}


glm::mat4 Camera::getView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getProj()
{
	return glm::perspective(glm::radians(fov), (double)resWidth / resHeight, 0.3, 100.0);
}

//direction
void Camera::updateDirection()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	cameraRight = glm::normalize(glm::cross(cameraFront, {0,1,0}));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

double Camera::offsetPitch(double _offset, double _sensitivity)
{
	pitch += _offset * _sensitivity;
	if (pitch >= 85) {
		pitch = 85.0f;
	}
	if (pitch <= -85) {
		pitch = -85.0f;
	}

	updateDirection();
	return pitch;
}

double Camera::offsetYaw(double _offset, double _sensitivity)
{
	yaw += _offset * _sensitivity;
	if (yaw < 0)
		yaw += 360;
	if (yaw > 360)
		yaw -= 360;
	updateDirection();
	return yaw;
}

//fov
double Camera::offsetFov(double _offset)
{
	if (fov + _offset > 90) {
		fov = 90.0f;
	}
	else if (fov + _offset < 1) {
		fov = 1.0f;
	}
	fov += _offset;
	return fov;
}

double Camera::getFov()
{
	return this->fov;
}

//position
void Camera::moveX(float _offset)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * _offset;
}

void Camera::moveY(float _offset)
{
	cameraPos += glm::normalize(cameraUp) * _offset;
}

void Camera::moveZ(float _offset)
{
	cameraPos += cameraFront * _offset;
}

void Camera::setRes(int w,int h){
	resWidth = w;
	resHeight = h;
	lastX = h / 2.0;
	lastY = w / 2.0;
}
