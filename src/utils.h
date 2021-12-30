#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>

#include "texture.h"
#include "shader.h"
#include "vao.h"

inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 mat) {
	glm::mat4 m;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			m[x][y] = mat[y][x];
		}
	}
	return m;
}

inline glm::vec3 assimpToGlmVec3(aiVector3D vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat) {
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;

	return q;
}

struct FreeCamera
{
	FreeCamera(glm::vec3 pos) : _position(pos) {}

	glm::vec3 _position = glm::vec3(0.f);
	glm::vec3 _front = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 _up = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 _right = glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 _world_up = glm::vec3(0.f, 1.f, 0.f);
	float _yaw = -90.f;
	float _pitch = 0.f;
	float _movement_speed = 0.2f;
	float _mouse_sensitivity = 0.1f;
	float _zoom = -65.f;

	float _mouse_last_x = NAN;
	float _mouse_last_y = NAN;

	glm::mat4 view_matrix() const
	{
		return glm::lookAt(_position, _position + _front, _up);
	}

	void on_keyboard_move(glm::vec3 delta)
	{
		float velocity = _movement_speed;
		_position += delta * velocity;
	}

	void on_mouse_scroll(float yoffset)
	{
		_zoom -= yoffset;
	}

	void on_mouse_move(float x, float y)
	{
		if (std::isnan(_mouse_last_x) || std::isnan(_mouse_last_y))
		{
			_mouse_last_x = x;
			_mouse_last_y = y;
		}
		const float xoffset = (x - _mouse_last_x) * _mouse_sensitivity;
		const float yoffset = (_mouse_last_y - y) * _mouse_sensitivity;
		_mouse_last_x = x;
		_mouse_last_y = y;
		_yaw += xoffset;
		_pitch += yoffset;
		force_refresh();
	}

	void force_refresh()
	{
		_pitch = std::min(std::max(_pitch, -89.0f), 89.0f);

		_front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front.y = sin(glm::radians(_pitch));
		_front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front = glm::normalize(_front);
		_right = glm::normalize(glm::cross(_front, _world_up));
		_up = glm::normalize(glm::cross(_right, _front));
	}
};

int WIDTH = 800;
int HEIGHT = 600;

struct AppState
{
	FreeCamera camera = FreeCamera(glm::vec3(0.0f, -5.0f, 7.0f));
};