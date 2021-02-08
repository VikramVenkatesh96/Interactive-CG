#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <string>
#include <vector>

class Window;

struct VulkanPhysicalDevices {
    std::vector<VkPhysicalDevice> m_devices;
    std::vector<VkPhysicalDeviceProperties> m_devProps;
    std::vector< std::vector<VkQueueFamilyProperties> > m_qFamilyProps;
    std::vector< std::vector<VkBool32> > m_qSupportsPresent;
    std::vector< std::vector<VkSurfaceFormatKHR> > m_surfaceFormats;
    std::vector<VkSurfaceCapabilitiesKHR> m_surfaceCaps;
};

class VulkanCore
{
public:
	VulkanCore(const char* appName);
	~VulkanCore();

	void Init(Window* pWindowControl);

    const VkPhysicalDevice& GetPhysDevice() const
    {
        return m_physDevices.m_devices[m_gfxDevIndex];
    }

    const VkSurfaceFormatKHR& GetSurfaceFormat() const
    {
        return m_physDevices.m_surfaceFormats[m_gfxDevIndex][m_gfxQueueFamily];
    }

    const VkSurfaceCapabilitiesKHR GetSurfaceCaps() const
    {
        return m_physDevices.m_surfaceCaps[m_gfxDevIndex];
    }

    const VkSurfaceKHR& GetSurface() const { return m_surface; }

    int GetQueueFamily() const { return m_gfxQueueFamily; }

    VkInstance& GetInstance() { return m_inst; }

    VkDevice& GetDevice() { return m_device; }

private:
    void CreateInstance();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();

private:
    VkInstance m_inst;
    VkDevice m_device;
    VkSurfaceKHR m_surface;
    VulkanPhysicalDevices m_physDevices;

    // Internal stuff
    std::string m_appName;
    int m_gfxDevIndex;
    int m_gfxQueueFamily;
};

