#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Frustum;
//!  A perspective camera with parameters that can be updated, outputs a view matrix
class Camera
{
private:
	int _width;
	int _height;

	glm::vec3 _position;
	float _yaw;
	float _pitch;
	glm::vec3 _front;
	glm::vec3 _up;
	glm::vec3 _right;
	float _fov;

	float _nearDistance;
	float _farDistance;

	glm::mat4 _projectionMatrix;
	float _lastX;
	float _lastY;

	Frustum *_frustum;

	void updateDirection();

public:
	/**
	 * Create a camera with the following parameters :
	 * \param position Its position
	 * \param fov The field of view (in degrees)
	 * \param width The camera viewport width in pixels
	 * \param height The camera viewport height in pixels
	 **/
	Camera(glm::vec3 position, float fov, int width, int height);

	//! Get current camera position
	inline const glm::vec3 &getPosition() const
	{
		return _position;
	}

	//! Get current camera direction vector
	inline const glm::vec3 &getDirection() const
	{
		return _front;
	}

	inline const glm::vec3 &getUpVector() const
	{
		return _up;
	}

	inline const glm::vec3 &getRightVector() const
	{
		return _right;
	}

	//! Get the homogenous 4x4 view matrix of the camera
	inline glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(_position, _position + _front, _up);
	}

	// ! Get projection perspective frustum
	inline glm::mat4 getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(_fov), (float)_width / _height, 0.3f, 100.0f);
	}

	//! Get current field of view (in degrees)
	inline const float &getFov() const
	{
		return this->_fov;
	}

	inline const float &getFarDistance() const
	{
		return this->_farDistance;
	}

	inline const float &getNearDistance() const
	{
		return this->_nearDistance;
	}

	//! Get camera viewport width (in px)
	int getResWidth() { return _width; };
	//! Get camera viewport height (in px)
	int getResHeight() { return _height; };

	//! Offset the pitch of camera with a sensitivity factor
	float offsetPitch(float _offset, float _sensitivity);
	//! Offset the pitch of camera with a sensitivity factor
	float offsetYaw(float _offset, float _sensitivity);
	//! Change the field of view of camera (from 1 to 90 degrees) by an offset
	float offsetFov(float _offset);

	void move(const glm::vec3 delta);

	void rotate(const glm::vec3 delta);
	//! Move camera in X axis my an offset
	void moveX(float _offset);
	//! Move camera in Y axis my an offset
	void moveY(float _offset);
	//! Move camera in Z axis my an offset
	void moveZ(float _offset);
	//! change
	void setRes(int w, int h);

	inline const Frustum *getFrustum() const { return _frustum; }
};

#endif
