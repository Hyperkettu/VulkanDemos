#pragma once

namespace Fox {

	namespace Vulkan {

		enum TextureType {
			NORMAL = 0,
			RENDER, 
			DEPTH,
			SWAPCHAIN
		};
	
		class Texture {
		public: 
			Texture() = default;
			Texture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
				VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);
			virtual ~Texture();

			void Create(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
				VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
			VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

			inline VkImage GetImage() {
				return image;
			}

			inline VkImageView GetImageView() {
				return imageView;
			}

		protected:

			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
			TextureType type;
		};

		class DepthTexture : public Fox::Vulkan::Texture {
		public: 
			DepthTexture() = default;
			DepthTexture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
				VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);
		private:
		};

		class RenderTexture : public Fox::Vulkan::Texture {
		public:
			RenderTexture() = default;
			RenderTexture(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
				VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);

			inline VkFramebuffer* GetFramebuffer() {
				return &framebuffer;
			}

		protected:
			VkFramebuffer framebuffer;
		};

		class SwapchainTexture : public Fox::Vulkan::Texture {
		public:
			SwapchainTexture() = default;
			SwapchainTexture(VkImage image, VkFormat imageFormat);
			~SwapchainTexture();
		
			inline VkFramebuffer* GetFramebuffer() {
				return &framebuffer;
			}

		protected:
			VkFramebuffer framebuffer;
		};
		
	}
}