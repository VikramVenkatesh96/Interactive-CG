#include "VulkanCore.h"
#include "Window.h"
#include <assert.h>
#include <stdint.h>

VulkanCore::VulkanCore(const char* appName):
    m_appName(appName)
{
}

VulkanCore::~VulkanCore()
{
    vkDestroySurfaceKHR(m_inst, m_surface, nullptr);
    vkDestroyInstance(m_inst, nullptr);
}

void VulkanEnumExtProps(std::vector<VkExtensionProperties>& extProps)
{
    uint32_t numExt = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &numExt, NULL);

    printf("Found %d extensions\n", numExt);

    extProps.resize(numExt);

    vkEnumerateInstanceExtensionProperties(NULL, &numExt, &extProps[0]);

    for (uint32_t i = 0; i < numExt; i++) {
        printf("Instance extension %d - %s\n", i, extProps[i].extensionName);
    }
}

void VulkanGetPhysicalDevices(const VkInstance& inst, const VkSurfaceKHR& Surface, VulkanPhysicalDevices& PhysDevices)
{
    uint32_t numDevices = 0;

    vkEnumeratePhysicalDevices(inst, &numDevices, NULL);
    printf("Num physical devices %d\n", numDevices);

    PhysDevices.m_devices.resize(numDevices);
    PhysDevices.m_devProps.resize(numDevices);
    PhysDevices.m_qFamilyProps.resize(numDevices);
    PhysDevices.m_qSupportsPresent.resize(numDevices);
    PhysDevices.m_surfaceFormats.resize(numDevices);
    PhysDevices.m_surfaceCaps.resize(numDevices);

    vkEnumeratePhysicalDevices(inst, &numDevices, &PhysDevices.m_devices[0]);

    for (uint32_t i = 0; i < numDevices; i++)
    {
        const VkPhysicalDevice& physDev = PhysDevices.m_devices[i];
        vkGetPhysicalDeviceProperties(physDev, &PhysDevices.m_devProps[i]);
        printf("Device name: %s\n", PhysDevices.m_devProps[i].deviceName);
        uint32_t apiVer = PhysDevices.m_devProps[i].apiVersion;
        printf("    API version: %d.%d.%d\n", VK_VERSION_MAJOR(apiVer),
            VK_VERSION_MINOR(apiVer),
            VK_VERSION_PATCH(apiVer));
        uint32_t numQFamily = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(physDev, &numQFamily, NULL);

        printf("    Num of family queues: %d\n", numQFamily);

        PhysDevices.m_qFamilyProps[i].resize(numQFamily);
        PhysDevices.m_qSupportsPresent[i].resize(numQFamily);

        vkGetPhysicalDeviceQueueFamilyProperties(physDev, &numQFamily, &(PhysDevices.m_qFamilyProps[i][0]));
        for (uint32_t q = 0; q < numQFamily; q++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physDev, q, Surface, &(PhysDevices.m_qSupportsPresent[i][q]));
        }

        uint32_t numFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, Surface, &numFormats, NULL);
        assert(numFormats > 0);

        PhysDevices.m_surfaceFormats[i].resize(numFormats);

        vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, Surface, &numFormats, &(PhysDevices.m_surfaceFormats[i][0]));

        for (uint32_t j = 0; j < numFormats; j++) {
            const VkSurfaceFormatKHR& SurfaceFormat = PhysDevices.m_surfaceFormats[i][j];
            printf("    Format %d color space %d\n", SurfaceFormat.format, SurfaceFormat.colorSpace);
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, Surface, &(PhysDevices.m_surfaceCaps[i]));
    }
}

void VulkanCore::Init(Window* pWindowControl)
{
    std::vector<VkExtensionProperties> extProps;
    VulkanEnumExtProps(extProps);
    CreateInstance();
    m_surface = pWindowControl->CreateSurface(m_inst);
    assert(m_surface);
    VulkanGetPhysicalDevices(m_inst, m_surface, m_physDevices);
    SelectPhysicalDevice();
    CreateLogicalDevice();
}

//const VkSurfaceFormatKHR& VulkanCore::GetSurfaceFormat() const
//{
//    // TODO: insert return statement here
//}
//
//const VkSurfaceCapabilitiesKHR VulkanCore::GetSurfaceCaps() const
//{
//    //TODO
//}

void VulkanCore::CreateInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = m_appName.c_str();
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instInfo.enabledExtensionCount = glfwExtensionCount;
    instInfo.ppEnabledExtensionNames = glfwExtensions;

    vkCreateInstance(&instInfo, NULL, &m_inst);
}

void VulkanCore::CreateSurface()
{
}

void VulkanCore::SelectPhysicalDevice()
{
    for (uint32_t i = 0; i < m_physDevices.m_devices.size(); i++) {

        for (uint32_t j = 0; j < m_physDevices.m_qFamilyProps[i].size(); j++) {
            VkQueueFamilyProperties& QFamilyProp = m_physDevices.m_qFamilyProps[i][j];

            printf("Family %d Num queues: %d\n", j, QFamilyProp.queueCount);
            VkQueueFlags flags = QFamilyProp.queueFlags;
            printf("    GFX %s, Compute %s, Transfer %s, Sparse binding %s\n",
                (flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
                (flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
                (flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
                (flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No");

            if (flags & VK_QUEUE_GRAPHICS_BIT) {
                if (!m_physDevices.m_qSupportsPresent[i][j]) {
                    printf("Present is not supported\n");
                    continue;
                }

                m_gfxDevIndex = i;
                m_gfxQueueFamily = j;
                printf("Using GFX device %d and queue family %d\n", m_gfxDevIndex, m_gfxQueueFamily);
                break;
            }
        }
    }

    if (m_gfxDevIndex == -1) {
        printf("No GFX device found!\n");
        assert(0);
    }
    else
    {
        //Override to use dedicated GPU as it usually is the first one in physical devices
        m_gfxDevIndex = 0;
    }
}

void VulkanCore::CreateLogicalDevice()
{
    float qPriorities = 1.0f;
    VkDeviceQueueCreateInfo qInfo = {};
    qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qInfo.queueFamilyIndex = m_gfxQueueFamily;
    qInfo.queueCount = 1;
    qInfo.pQueuePriorities = &qPriorities;

    const char* pDevExt[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo devInfo = {};
    devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devInfo.enabledExtensionCount = 1; //need to change to pDevExtSize
    devInfo.ppEnabledExtensionNames = pDevExt;
    devInfo.queueCreateInfoCount = 1;
    devInfo.pQueueCreateInfos = &qInfo;

    vkCreateDevice(GetPhysDevice(), &devInfo, NULL, &m_device);

    printf("Device created\n");
}
