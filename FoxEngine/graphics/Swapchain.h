#pragma once

namespace Fox {

	namespace Vulkan {
		
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		class Swapchain {
		public: 
			Swapchain() {};
			~Swapchain();

			VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			SwapChainSupportDetails QuerySupport(VkPhysicalDevice device, VkSurfaceKHR surface);
			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
			void Cleanup();
			void Recreate(VkRenderPass renderPass);
			void Create();
			void CreateColorResources();
			void CreateDepthResources();
			void CreateFrameBuffers(VkRenderPass renderPass);


			inline VkFramebuffer GetFramebuffer(uint32_t imageIndex) {
				return *textures[imageIndex]->GetFramebuffer();
			}

			inline VkExtent2D GetExtent() {
				return extent;
			}

			inline VkFormat GetImageFormat() {
				return imageFormat;
			}

			VkSwapchainKHR swapChain;

		private: 

	//		std::vector<VkImage> images;
	//		std::vector<VkImageView> imageViews;
	//		std::vector<VkFramebuffer> framebuffers;

			std::vector<std::unique_ptr<Fox::Vulkan::SwapchainTexture>> textures;

			VkFormat imageFormat;
			VkExtent2D extent;

			//VkImage colorImage;
			//VkDeviceMemory colorImageMemory;
			//VkImageView colorImageView;

		//	VkImage depthImage;
		//	VkDeviceMemory depthImageMemory;
		//	VkImageView depthImageView;
			std::shared_ptr<Fox::Vulkan::RenderTexture> renderTexture;
			std::shared_ptr<Fox::Vulkan::DepthTexture> depthTexture;
		};
	}
}