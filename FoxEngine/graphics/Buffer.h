#pragma once

namespace Fox {

	namespace Vulkan {

        class Renderer;

		template<class T>
		class Buffer {
		public: 
			Buffer() {}
		    ~Buffer();

			void Create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
			void CopyImage(VkDeviceSize imageSize, unsigned char* pixels);
			void CopyData(std::vector<T>& data, VkDeviceSize size);
			void Map();
			void Update(T& data);
            void SetContents(const std::vector<T>& content, bool sendToGPU = true);
            size_t GetElementCount() const {
                return content.size();
            }

			VkBuffer GetBuffer() {
				return buffer;
			}

		protected: 
			VkBuffer buffer;
			VkDeviceMemory bufferMemory;
			VkDeviceSize size;

            std::vector<T> content;
			void* mappedMemory;
		};

        template<class T>
        Buffer<T>::~Buffer() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkDestroyBuffer(device, buffer, nullptr);
            vkFreeMemory(device, bufferMemory, nullptr);
        }

        template<class T>
        void Buffer<T>::Create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            this->size = size;

            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = Fox::Vulkan::Renderer::GetRenderer()->findMemoryType(memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(device, buffer, bufferMemory, 0);
        }

        template<class T>
        void Buffer<T>::CopyImage(VkDeviceSize imageSize, unsigned char* pixels) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            void* data;
            vkMapMemory(device, bufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device, bufferMemory);
        }

        template<class T>
        void Buffer<T>::Map() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkMapMemory(device, bufferMemory, 0, size, 0, &mappedMemory);

        }

        template<class T>
        void Buffer<T>::CopyData(std::vector<T>& data, VkDeviceSize bufferSize) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &mappedMemory);
            memcpy(mappedMemory, data.data(), (size_t)bufferSize);
            vkUnmapMemory(device, bufferMemory);
        }

        template<class T>
        void Buffer<T>::Update(T& data) {
            memcpy(mappedMemory, &data, sizeof(data));
        }

        template<class T>
        void Buffer<T>::SetContents(const std::vector<T>& contents, bool sendToGPU) {
            content.resize(contents.size());
            std::copy(contents.begin(), contents.end(), content.begin());
            size = sizeof(T) * content.size();
            if (sendToGPU) {
                CopyData(content, size);
            }

        }

	}
}