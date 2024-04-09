#pragma once

#include <vulkan/vulkan.h>

namespace Fox {

	namespace Vulkan {

		template<class T>
		class Buffer;

		class Renderer {

			public:
				Renderer() {
					instance = this;
				}
				~Renderer();

				static VkDevice GetDevice() {
					return instance->device;
				}

				static Renderer* GetRenderer() {
					return instance;
				}

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

				void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

				VkCommandBuffer beginSingleTimeCommands();
				void endSingleTimeCommands(VkCommandBuffer commandBuffer);

				void DeleteBuffers() {
		//			delete vertexBuffer;
		//			delete indexBuffer;

					for (size_t i = 0u; i < uniformBuffers.size(); i++) {
						delete uniformBuffers[i];
					}
				}

			public:

				static Renderer* instance;



				VkDevice device;
				VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	//		Fox::Vulkan::Buffer<Vertex>* vertexBuffer;
	//		Fox::Vulkan::Buffer<uint32_t>* indexBuffer;
			std::vector<Fox::Vulkan::Buffer<UniformBufferObject>*> uniformBuffers;
			VkCommandPool commandPool;
			VkQueue graphicsQueue;
		};
	}
}