#include "Window.h"
#ifdef _WIN64
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

bool Window::CreateNewWindow()
{
#ifdef _WIN64
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	windowName.append(" - Vulkan");
#else
	windowName.append(" - OpenGL");
#endif
	glWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	if (!glWindow)
	{
		return false;
	}
	glfwMakeContextCurrent(glWindow);
	glfwSetKeyCallback(glWindow,key_callback);

	return true;
}

bool Window::ShouldWindowClose()
{
	return glfwWindowShouldClose(glWindow);
}

void Window::UpdateWindow()
{
#ifdef _WIN64

#else
	glfwSwapBuffers(glWindow);
#endif // _WIN64
	glfwPollEvents();
}

void Window::DestroyWindow()
{
	if (glWindow)
	{
		glfwDestroyWindow(glWindow);
	}
}
#ifdef _WIN64
VkSurfaceKHR Window::CreateSurface(VkInstance& inst)
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = glfwGetWin32Window(glWindow);
	createInfo.hinstance = GetModuleHandle(nullptr);

	VkSurfaceKHR surface;
	vkCreateWin32SurfaceKHR(inst, &createInfo, NULL, &surface);
	return surface;
}
#endif // _WIN64

