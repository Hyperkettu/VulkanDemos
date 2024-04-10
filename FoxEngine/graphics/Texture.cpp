#include "pch.h"

namespace Fox {

	namespace Vulkan {

		Texture::Texture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
            VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags) {
            Create(width, height, mipLevels, numSamples, imageFormat, tiling, usage, properties, image, imageMemory);
			imageView = CreateImageView(image, imageFormat, aspectFlags, mipLevels);
            type = Fox::Vulkan::TextureType::NORMAL;
		}

		Texture::~Texture() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            if (type == Fox::Vulkan::TextureType::SWAPCHAIN) {
                return;
            }
            vkDestroyImageView(device, imageView, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, imageMemory, nullptr);
        }
	
        void Texture::Create(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {

            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = mipLevels;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.samples = numSamples;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device, image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = Fox::Vulkan::Renderer::GetRenderer()->findMemoryType(memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate image memory!");
            }

            vkBindImageMemory(device, image, imageMemory, 0);
        }

        VkImageView Texture::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            VkImageView imageView;
            if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }

            return imageView;
        }

        RenderTexture::RenderTexture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
            VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags) :
            Fox::Vulkan::Texture(width, height, mipLevels, numSamples, imageFormat, tiling, usage, properties, aspectFlags) {
            type = Fox::Vulkan::TextureType::RENDER;
        }

        DepthTexture::DepthTexture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
            VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags) :
            Fox::Vulkan::Texture(width, height, mipLevels, numSamples, imageFormat, tiling, usage, properties, aspectFlags) {
            type = Fox::Vulkan::TextureType::DEPTH;
        }

        SwapchainTexture::SwapchainTexture(VkImage image, VkFormat imageFormat) {
            this->image = image;
            imageView = CreateImageView(image, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            type = Fox::Vulkan::TextureType::SWAPCHAIN; 
        }

        SwapchainTexture::~SwapchainTexture() {
        }
        
    }
	
}