#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraInfo
{
	std::optional<glm::vec3> position;
	std::optional<float> fov;
	std::optional<float> yaw;
	std::optional<float> pitch;
};

struct Frustum;
class Scene;
//!  A perspective camera with parameters that can be updated, outputs a view matrix
class Camera
{
private:
	int width;
	int height;

	glm::vec3 movingFactor;
	float moveSpeed;
	glm::vec3 position;
	float yaw;
	float pitch;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float fov;

	float nearDistance;
	float farDistance;

	glm::mat4 projectionMatrix;
	float lastX;
	float lastY;
	Frustum *frustum;
	bool constantSpeed = false;

	

public:
	/**
	 * Create a camera with the following parameters :
	 * \param width The camera viewport width in pixels
	 * \param height The camera viewport height in pixels
	 * \param position Its position
	 * \param fov The field of view (in degrees)
	 **/
	Camera(int _width, int _height, glm::vec3 _position = glm::vec3{0.0f, 0.0f, 0.0f}, float _fov = 55.0f);

	~Camera()
	{
	}

	inline void setMoveSpeed(float _speed)
	{
		moveSpeed = _speed;
	}

	void updateDirection();

	inline float getMoveSpeed()
	{
		return moveSpeed;
	}

	//! Get current camera position
	inline const glm::vec3 &getPosition() const
	{
		return position;
	}

	//! Get current camera direction vector
	inline const glm::vec3 &getDirection() const
	{
		return front;
	}

	inline const glm::vec3 &getUpVector() const
	{
		return up;
	}

	inline const glm::vec3 &getRightVector() const
	{
		return right;
	}

	//! Get the homogenous 4x4 view matrix of the camera
	inline glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(position, position + front, up);
	}

	// ! Get projection perspective frustum
	inline glm::mat4 getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(fov), (float)width / height, nearDistance, farDistance);
	}

	//! Get current field of view (in degrees)
	inline const float &getFov() const
	{
		return this->fov;
	}

	inline const float &getFarDistance() const
	{
		return this->farDistance;
	}

	inline const float &getNearDistance() const
	{
		return this->nearDistance;
	}

	inline const float &getYaw() const
	{
		return this->yaw;
	}

	inline const float &getPitch() const
	{
		return this->pitch;
	}

	inline bool isMoving() const { return !glm::epsilonEqual(movingFactor.x, 0.0f, glm::epsilon<float>()) || !glm::epsilonEqual(movingFactor.y, 0.0f, glm::epsilon<float>()) || !glm::epsilonEqual(movingFactor.z, 0.0f, glm::epsilon<float>()); }

	//! Get camera viewport width (in px)
	int getResWidth() const { return width; };

	//! Get camera viewport height (in px)
	int getResHeight() const { return height; };

	//! Offset the pitch of camera with a sensitivity factor
	float offsetPitch(float _offset, float _sensitivity);
	//! Offset the pitch of camera with a sensitivity factor
	float offsetYaw(float _offset, float _sensitivity);
	//! Change the field of view of camera (from 1 to 90 degrees) by an offset
	float offsetFov(float _offset);

	void move(const float delta);

	//! Move camera in X axis my an offset
	void moveX(float _offset);
	//! Move camera in Y axis my an offset
	void moveY(float _offset);
	//! Move camera in Z axis my an offset
	void moveZ(float _offset);
	//! change
	void setResolution(int _w, int _h);

	inline const Frustum *getFrustum() const { return frustum; }

	void setCameraInfo(CameraInfo _cameraInfo);

	void setConstantSpeed(bool _status){
		constantSpeed = _status;
	}
};

#endif
