#ifndef CAMERA_H
#define CAMERA_H

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif
//!  A perspective camera with parameters that can be updated, outputs a view matrix
class Camera {
private:
	glm::vec3 cameraPos; 
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
 	double yaw;
 	double pitch;
 	double lastX;
 	double lastY;
 	double fov;

	int resWidth;
	int resHeight;

	void updateDirection();
	
public:
    /**
	 * Create a camera with the following parameters :
	 * \param _cameraPos It's position
	 * \param _fov The field of view (in degrees)
	 * \param _resWidth The camera viewport width in pixels
	 * \param _resHeight The camera viewport height in pixels
	 **/
	Camera(){};
	Camera(glm::vec3 _cameraPos, double _fov, int _resWidth, int _resHeight);

	//! Get current camera position 
	glm::vec3 getPos();
	//! Get current camera direction vector
	glm::vec3 getDir();
	//! Get the homogenous 4x4 view matrix of the camera
	glm::mat4 getView();
	// ! Get projection perspective frustum
	glm::mat4 getProj();
	//! Get current field of view (in degrees)
	double getFov();

	//! Get camera viewport width (in px)
	int getResWidth(){return resWidth;};
	//! Get camera viewport height (in px)
	int getResHeight(){return resHeight;};

	//! Offset the pitch of camera with a sensitivity factor
	double offsetPitch(double _offset, double _sensitivity);
	//! Offset the pitch of camera with a sensitivity factor
	double offsetYaw(double _offset, double _sensitivity);
	//! Change the field of view of camera (from 1 to 90 degrees) by an offset
	double offsetFov(double _offset);

	//! Move camera in X axis my an offset
	void moveX(float _offset);
	//! Move camera in Y axis my an offset
	void moveY(float _offset);
	//! Move camera in Z axis my an offset
	void moveZ(float _offset);
	//! change
	void setRes(int w,int h);
};

#endif
