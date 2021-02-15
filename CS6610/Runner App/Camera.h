#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

class Camera
{
public:
	Camera(const glm::vec3 pos, float FOV, float AR, float zNear, float zFar ):
		pitch(0),
		yaw(0)
	{
		m_perspective = glm::perspective(FOV, AR, zNear, zFar);
		m_pos = pos;
		m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	inline glm::mat4 GetViewMatrix() const
	{
		return m_perspective * glm::lookAt(m_pos, m_pos + m_forward, m_up);
	}

	inline void pitchCamera(const float pitch_val)
	{
		m_forward = glm::rotate(
			glm::radians(pitch_val),
			glm::normalize(glm::cross(m_forward, m_up))
		) * glm::vec4(m_forward, 1);
	}

	inline void yawCamera(const float yaw_val)
	{
		m_forward = glm::rotate(
			glm::radians(yaw_val),
			glm::normalize(m_up)
		) * glm::vec4(m_forward, 1);
	}

	enum class Direction {FORWARD,BACKWARD,LEFT,RIGHT};

	inline void Translate(const Direction direction,const float deltaTime)
	{
		glm::vec3 directionToMove;
		if (direction == Direction::FORWARD)
			directionToMove = m_forward;
		else if (direction == Direction::BACKWARD)
			directionToMove = -m_forward;
		else if (direction == Direction::LEFT)
			directionToMove = -glm::cross(m_forward,m_up);
		else if (direction == Direction::RIGHT)
			directionToMove = glm::cross(m_forward, m_up);

		m_pos += directionToMove * speed * deltaTime;
	}
	inline float GetSpeed() { return speed; }
	inline void SetSpeed(const float& newSpeed) { speed = newSpeed; }
private:
	glm::mat4 m_perspective;
	glm::vec3 m_pos;
	float pitch;
	float yaw;
	float speed = 10.0f;
	glm::vec3 m_forward;
	glm::vec3 m_up;
};

