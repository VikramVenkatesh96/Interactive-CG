#pragma once
#include "Window.h"
#ifdef _WIN64
#include "VulkanCore.h"
#endif // _WIN64

class Application
{
public:

	Application(uint32_t width, uint32_t height, const char* appName) :
#ifdef _WIN64
		m_core(appName),
#endif // DEBUG
		window(width,height,appName)
	{}

	inline void Run()
	{
		Init();
		Update();
		Cleanup();
	}
private:
	void Init();
	void Update();
	void Cleanup();
private:
	Window window;
#ifdef _WIN64
private:
	void CreateSwapChain();
	void CreateCommandBuffer();
	void RecordCommandBuffers();

private:
	VulkanCore m_core;
	std::vector<VkImage> m_images;
	VkSwapchainKHR m_swapChain;
	VkQueue m_queue;
	std::vector<VkCommandBuffer> m_cmdBufs;
	VkCommandPool m_cmdBufPool;
#endif
};

