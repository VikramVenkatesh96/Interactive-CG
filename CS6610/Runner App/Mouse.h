#pragma once
#include <GLFW\glfw3.h>
#include <stdio.h>
class Mouse
{
public:
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			m_isDraggingLMB = true;
			m_firstClickLMB = false;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			m_isDraggingLMB = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			m_isDraggingRMB = true;
			m_firstClickRMB = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{
			m_isDraggingRMB = false;
		}
	}
	inline static void MouseCursorCallback(GLFWwindow* window, double xPos, double yPos)
	{
		if (m_isDraggingLMB)
		{
			printf("x: %f, y%f\n", xPos,yPos);
		}
	}

	inline static bool isDraggingLMB() { return m_isDraggingLMB; }
	inline static bool isDraggingRMB() { return m_isDraggingRMB; }
	inline static bool isFirstClickLMB() { return m_firstClickLMB; }
	inline static bool isFirstClickRMB() { return m_firstClickRMB; }
	inline static float GetSensitivity() { return m_sensitivity; }
private:
	static bool m_isDraggingLMB;
	static bool m_isDraggingRMB;
	static bool m_firstClickLMB;
	static bool m_firstClickRMB;
	static float m_sensitivity;
};


