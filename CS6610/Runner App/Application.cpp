#include "Application.h"
#include "Importer.h"
#include "Mouse.h"
#include <assert.h>

//Frametime counter(1/FPS)
double frameTime;
//Mesh and shader declaration
Mesh3D* mesh = nullptr;
Shader* shader = nullptr;
Camera* camera = nullptr;

static void CustomMouseCursorCallback(GLFWwindow* window, double xPos, double yPos)
{
	static double lastXPos, lastYPos;
	
	if (Mouse::isFirstClickRMB())
	{
		lastXPos = xPos;
		lastYPos = yPos;
	}
	if (Mouse::isDraggingRMB())
	{
		float delta_x = (float)(lastXPos - xPos) * Mouse::GetSensitivity();
		float delta_y = (float)(lastYPos- yPos) * Mouse::GetSensitivity();
		camera->yawCamera(delta_x);
		camera->pitchCamera(delta_y);
		lastXPos = xPos;
		lastYPos = yPos;
	}
	if (Mouse::isDraggingLMB())
	{
		float delta_x = (float)(lastXPos - xPos) * Mouse::GetSensitivity();
		float delta_y = (float)(lastYPos - yPos) * Mouse::GetSensitivity();

		lastXPos = xPos;
		lastYPos = yPos;
	}
}

static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{

	if (Mouse::isDraggingRMB())
	{
		static constexpr float speedFactor = 2.0f;
		camera->SetSpeed(camera->GetSpeed() * ((yOffset > 0) ? speedFactor : 1.0f / speedFactor));
	}
	else
	{
		camera->Translate(((yOffset > 0) ? Camera::Direction::FORWARD : Camera::Direction::BACKWARD),
			(float)frameTime);
	}
}

static void CustomKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void HandleKeyboardInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->Translate(Camera::Direction::FORWARD, (float)frameTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->Translate(Camera::Direction::LEFT, (float)frameTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->Translate(Camera::Direction::BACKWARD, (float)frameTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->Translate(Camera::Direction::RIGHT, (float)frameTime);
	}
}

void Application::Init()
{
	if (!glfwInit())
	{
		exit(-1);
	}

	if (!window.CreateNewWindow())
	{
		glfwTerminate();
		exit(-1);
	}

	glfwSetMouseButtonCallback(window.GetGLWindow(), Mouse::MouseButtonCallback);
	glfwSetCursorPosCallback(window.GetGLWindow(), CustomMouseCursorCallback);
	glfwSetScrollCallback(window.GetGLWindow(), ScrollCallback);
	glfwSetKeyCallback(window.GetGLWindow(), CustomKeyCallback);
	camera = new Camera(glm::vec3(0.0f ,0.0f ,-5.0f), 70.0f, window.GetAspectRatio(), 0.01f, 1000.0f);
	mesh = Importer::Mesh::ImportByFileName("teapot.obj");
	mesh->GetTransform().SetScale(glm::vec3(0.05f));
	shader = new Shader("Standard.glsl", "Standard.glsl");
	
#ifdef _WIN64
	m_core.Init(&window);
	vkGetDeviceQueue(m_core.GetDevice(), m_core.GetQueueFamily(), 0, &m_queue);
	CreateSwapChain();
	CreateCommandBuffer();
	RecordCommandBuffers();
#endif
}

void Application::Update()
{
	printf("Hit F6 to hot-reload shaders\n");
	
	double prevTime = glfwGetTime();
	while (!window.ShouldWindowClose())
	{
		static double timer = 0.0f;
		double currentTime = glfwGetTime();

		HandleKeyboardInput(window.GetGLWindow());
#ifdef _WIN64
		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(m_core.GetDevice(), m_swapChain, UINT64_MAX, NULL, NULL, &imageIndex);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_cmdBufs[imageIndex];
		
		vkQueueSubmit(m_queue, 1, &submitInfo, NULL);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapChain;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(m_queue, &presentInfo);
		//Prepare command buffer for next images
		RecordCommandBuffers();
#else
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//Update mesh
		mesh->GetTransform().SetRot(glm::vec3(glm::radians(-90.0f) , currentTime, 0.0f));
		shader->Update(mesh->GetTransform(), *camera);
		mesh->Draw(shader);
#endif // _WIN64
		window.UpdateWindow();
		
		if (glfwGetKey(window.GetGLWindow(), GLFW_KEY_F6))
		{
			//Mechanism to lock reloading shaders for some time.
			//Have to wait atleast 1 sec before you can press F6 again.
			if (timer > 1.0f)
			{
				shader->Hot_Reload("Standard.glsl", "Standard.glsl");
				printf("Recompiled Shaders!\n");
				timer = 0;
			}
		}
		frameTime = currentTime - prevTime;
		timer += frameTime;
		prevTime = currentTime;
	}
}

void Application::Cleanup()
{
#ifdef _WIN64
	vkFreeCommandBuffers(m_core.GetDevice(), m_cmdBufPool, m_images.size(), &m_cmdBufs[0]);
	vkDestroyCommandPool(m_core.GetDevice(), m_cmdBufPool, NULL);
	vkDestroySwapchainKHR(m_core.GetDevice(),m_swapChain,NULL);
#endif
	delete mesh;
	delete shader;
	delete camera;
	window.DestroyWindow();
	glfwTerminate();
}

#ifdef _WIN64
void Application::CreateSwapChain()
{
	const VkSurfaceCapabilitiesKHR& surfaceCaps = m_core.GetSurfaceCaps();
	assert(surfaceCaps.currentExtent.width != -1);

	uint32_t numImages = 2; //Double buffering like OpenGL swap buffers
	assert(numImages >= surfaceCaps.minImageCount && numImages <= surfaceCaps.maxImageCount);
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};

	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = m_core.GetSurface();
	swapChainCreateInfo.minImageCount = numImages;
	swapChainCreateInfo.imageFormat = m_core.GetSurfaceFormat().format;
	swapChainCreateInfo.imageColorSpace = m_core.GetSurfaceFormat().colorSpace;
	swapChainCreateInfo.imageExtent = surfaceCaps.currentExtent;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapChainCreateInfo.clipped = true;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vkCreateSwapchainKHR(m_core.GetDevice(), &swapChainCreateInfo, NULL, &m_swapChain);

	uint32_t numImagesInSwapChain;
	vkGetSwapchainImagesKHR(m_core.GetDevice(), m_swapChain, &numImagesInSwapChain, NULL);
	m_images.resize(numImagesInSwapChain);
	m_cmdBufs.resize(numImagesInSwapChain);

	vkGetSwapchainImagesKHR(m_core.GetDevice(), m_swapChain, &numImagesInSwapChain, &(m_images[0]));
}

void Application::CreateCommandBuffer()
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = m_core.GetQueueFamily();

	vkCreateCommandPool(m_core.GetDevice(), &cmdPoolCreateInfo, NULL, &m_cmdBufPool);

	VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
	cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocInfo.commandPool = m_cmdBufPool;
	cmdBufAllocInfo.commandBufferCount = m_images.size();
	cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	vkAllocateCommandBuffers(m_core.GetDevice(), &cmdBufAllocInfo, &m_cmdBufs[0]);
}

void Application::RecordCommandBuffers()
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkClearColorValue clearColor = { fabs(sinf(glfwGetTime())), 0.0f, 0.0f, 1.0f };
	VkClearValue clearValue = {};
	clearValue.color = clearColor;

	VkImageSubresourceRange imageRange = {};
	imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageRange.levelCount = 1;
	imageRange.layerCount = 1;

	for (uint32_t i = 0; i < m_cmdBufs.size(); i++) {
		vkBeginCommandBuffer(m_cmdBufs[i], &beginInfo);

		vkCmdClearColorImage(m_cmdBufs[i], m_images[i], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);

		vkEndCommandBuffer(m_cmdBufs[i]);
	}
}

#endif