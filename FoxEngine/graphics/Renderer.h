#pragma once

#include <vulkan/vulkan.h>

namespace Fox {

	namespace Vulkan {

		class Model;
		class Swapchain;
		class Texture;
		class Synchronization;
		class DescriptorSetManager;
		class SamplerManager;
		class ConstantBuffers;
		class TextureManager;
		class GraphicsPipelineStateManager;
		class RenderPassManager;
		class SceneGraph;
		class SceneNode;

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		struct Batch {
			Fox::Vulkan::Model* model;
			glm::mat4 matrix;
		};

		template<class T>
		class Buffer;

		class Renderer {

			public:
				Renderer() {
					rendererInstance = this;
				}
				~Renderer();

				static VkDevice GetDevice() {
					return rendererInstance->device;
				}

				static Renderer* GetRenderer() {
					return rendererInstance;
				}

				void Initialize();
				void Initialize(const Fox::Vulkan::RendererConfig& config);
				void Destroy();

				void Render();
				void RenderBegin(VkCommandBuffer commandBuffer);
				void RenderEnd(VkCommandBuffer commandBuffer);
				void DrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
				void SetDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* descriptorSets);
				void SetIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer indexBuffer, VkDeviceSize offset, VkIndexType type);
				void SetVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkBuffer>& vertexBuffers, uint32_t firstBinding, const VkDeviceSize* offsets);
				void SetScissor(VkCommandBuffer commandBuffer, VkOffset2D offset, VkExtent2D extent);
				void SetViewport(VkCommandBuffer commandBuffer, float topleftX, float topleftY, float width, float height, float minDepth, float maxDepth);
				void SetGraphicsPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline);
				void RenderPassBegin(VkCommandBuffer commandBuffer, VkClearColorValue clearColor, float clearDepth, uint32_t clearStencil, VkRenderPass renderPass, VkFramebuffer framebuffer, VkOffset2D renderAreaOffset, VkExtent2D renderArea);
				void RenderPassEnd(VkCommandBuffer);

				uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
					VkPhysicalDeviceMemoryProperties memProperties;
					vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

					for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
						if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
							return i;
						}
					}

					throw std::runtime_error("Failed to find suitable memory type!");
				}

				void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
				void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

				VkCommandBuffer BeginSingleTimeCommands();
				void EndSingleTimeCommands(VkCommandBuffer commandBuffer);



				static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
					VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
					VkDebugUtilsMessageTypeFlagsEXT messageType,
					const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
					void* pUserData);


				VkSampleCountFlagBits GetMaxUsableSampleCount();


				VkShaderModule CreateShaderModule(const std::vector<char>& code) {
					VkShaderModuleCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
					createInfo.codeSize = code.size();
					createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
					VkShaderModule shaderModule;
					if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
						throw std::runtime_error("Failed to create shader module!");
					}
					return shaderModule;
				}

				QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

				bool IsDeviceSuitable(VkPhysicalDevice device);
				void PickPhysicalDevice();
				void CreateLogicalDevice();


				void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
				void CreateCommandBuffers();
				void CreateCommandPool(); 

				VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
				VkFormat FindDepthFormat();
				bool HasStencilComponent(VkFormat format);				
				void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

				void ResizeWindow(int width, int height) {
					if (screenWidth != width || screenHeight != height) {
						if (screenWidth != ~0u && screenHeight != ~0u) {
							frameBufferResized = true;
						}
					}
					screenWidth = width;
					screenHeight = height;
				}

				void LoadModel();

				Fox::Vulkan::RendererConfig& GetConfig() {
					return config;
				}

				inline Fox::Vulkan::SamplerManager* GetSamplerManager() {
					return samplerManager.get();
				}

				inline Fox::Vulkan::TextureManager* GetTextureManager() {
					return textureManager.get();
				}

				inline Fox::Vulkan::ConstantBuffers* GetConstantBuffers() {
					return constantBuffers.get();
				}

				inline Fox::Vulkan::DescriptorSetManager* GetDesciptorManager() {
					return descriptorManager.get();
				}

				inline Fox::Vulkan::RenderPassManager* GetRenderPassManager() {
					return renderPassManager.get();
				}

				VkSurfaceKHR surface;
				VkInstance instance;
				std::shared_ptr<Fox::Vulkan::SceneGraph> sceneGraph;

				Fox::Vulkan::SceneNode* model;
				float angle = 0.0f;


				std::vector<Fox::Vulkan::Batch> batches;
				
		private:

			void CreateVulkanInstance();
			void CreateSDL2Surface(SDL_Window* window);
			std::vector<const char*> GetRequiredExtensions();
			bool CheckValidationLayerSupport();
			bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
			void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
			void SetupDebugMessenger();
			void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		
			const std::string MODEL_PATH = "models/viking.obj";

#ifdef NDEBUG
			const bool enableValidationLayers = false;
#else
			const bool enableValidationLayers = true;
#endif

			const std::vector<const char*> validationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};

			const std::vector<const char*> deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			public:
				static Renderer* rendererInstance;

				VkDevice device;
				VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

			VkCommandPool commandPool;
			VkQueue graphicsQueue;


			VkDebugUtilsMessengerEXT debugMessenger;

			VkQueue presentQueue;

			std::vector<VkCommandBuffer> commandBuffers; // cleaned with pool automatically

			uint32_t currentFrame = 0u;

			uint32_t mipLevels;

			bool frameBufferResized = false;
			uint32_t screenWidth = ~0u; 
			uint32_t screenHeight = ~0u;

			const int MAX_FRAMES_IN_FLIGHT = 2;

			Fox::Vulkan::RendererConfig config;

			std::unique_ptr<Fox::Vulkan::DescriptorSetManager> descriptorManager;
			std::unique_ptr<Fox::Vulkan::SamplerManager> samplerManager;
			std::unique_ptr<Fox::Vulkan::TextureManager> textureManager;
			std::unique_ptr<Fox::Vulkan::RenderPassManager> renderPassManager;
			std::unique_ptr<Fox::Vulkan::GraphicsPipelineStateManager> graphicsPipelineState;
			std::unique_ptr<Fox::Vulkan::ConstantBuffers> constantBuffers;
			std::unique_ptr<Fox::Vulkan::Swapchain> swapchain;
			std::unique_ptr<Fox::Vulkan::Synchronization> synchronization;

	//		std::shared_ptr<Fox::Vulkan::Model> model;
		};
	}
}