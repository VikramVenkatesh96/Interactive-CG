#pragma once
#ifdef _WIN64
	#define VK_USE_PLATFORM_WIN32_KHR
	#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <string>

class Window
{
public:
	Window() {}
	Window(uint32_t i_width, uint32_t i_height, const char* i_windowName):
		width(i_width),
		height(i_height),
		windowName(i_windowName)
	{		
	}
	~Window()
	{
		DestroyWindow();
	}
	
	bool CreateNewWindow();
	bool ShouldWindowClose();
	void UpdateWindow();
	void DestroyWindow();

	inline const char* GetAppName()
	{
		return windowName.c_str();
	}

#ifdef _WIN64
	VkSurfaceKHR CreateSurface(VkInstance& inst);
#endif
	
private:
	uint32_t  width = 800u;
	uint32_t  height = 600u;
	std::string windowName;
	GLFWwindow* glWindow = nullptr;
};

