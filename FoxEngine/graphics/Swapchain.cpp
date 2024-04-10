#include "pch.h"

namespace Fox {

	namespace Vulkan {

        Swapchain::~Swapchain() {}

	VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

    SwapChainSupportDetails Swapchain::QuerySupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void Swapchain::Cleanup() {
        VkDevice device = Fox::Vulkan::Renderer::GetDevice();
        renderTexture = nullptr;
        depthTexture = nullptr;

        for (size_t i = 0; i < textures.size(); i++) {
            vkDestroyFramebuffer(device, *textures[i]->GetFramebuffer(), nullptr);
            vkDestroyImageView(device, textures[i]->GetImageView(), nullptr);

        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void Swapchain::Recreate(VkRenderPass renderPass) {
        VkDevice device = Fox::Vulkan::Renderer::GetDevice();

        vkDeviceWaitIdle(device);

        Cleanup();

        Create();
        CreateColorResources();
        CreateDepthResources();
        CreateFrameBuffers(renderPass);
    }

    VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();
            int width, height;
            SDL_Vulkan_GetDrawableSize(static_cast<SDL_Window*>(renderer->GetConfig().windowHandle), &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkPresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    void Swapchain::CreateColorResources() {
        Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();
        VkFormat colorFormat = imageFormat;

        renderTexture = std::make_shared<Fox::Vulkan::RenderTexture>(extent.width, extent.height, 1, renderer->GetConfig().msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Swapchain::Create() {
        VkDevice device = Fox::Vulkan::Renderer::GetDevice();
        Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

        SwapChainSupportDetails swapChainSupport = QuerySupport(renderer->physicalDevice, renderer->surface);

        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1u;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = renderer->surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // for post-processing VK_IMAGE_USAGE_TRANSFER_DST_BIT 

        QueueFamilyIndices indices = renderer->FindQueueFamilies(renderer->physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE; // for resizing need to be specified

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);

        std::vector<VkImage> images;
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

        textures.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            textures[i] = std::make_unique<Fox::Vulkan::SwapchainTexture>(images[i],surfaceFormat.format);
        }

        imageFormat = surfaceFormat.format;
        this->extent = extent;
    }

    void Swapchain::CreateDepthResources() {
        Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();
        VkFormat depthFormat = renderer->FindDepthFormat();
        depthTexture = std::make_shared<Fox::Vulkan::DepthTexture>(extent.width, extent.height, 1,
            renderer->GetConfig().msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
        renderer->TransitionImageLayout(depthTexture->GetImage(), depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    void Swapchain::CreateFrameBuffers(VkRenderPass renderPass) {
        VkDevice device = Fox::Vulkan::Renderer::GetDevice();

        for (size_t i = 0; i < textures.size(); i++) {

            std::array<VkImageView, 3> attachments = { renderTexture->GetImageView(), depthTexture->GetImageView(), textures[i]->GetImageView()
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;


            VkFramebuffer* framebuffer = textures[i]->GetFramebuffer();
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, framebuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }

    }

   
}
}