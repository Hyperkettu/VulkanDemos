#include "pch.h"

namespace Fox {

	namespace Vulkan {

        ConstantBuffers::~ConstantBuffers() {
            DeleteBuffers();
        }

        void ConstantBuffers::CreateUniformBuffers(uint32_t maxFramesInFlight) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            VkDeviceSize bufferSize = sizeof(Fox::Vulkan::PerFrameConstantBuffer);

            perFrame.resize(maxFramesInFlight);

            for (size_t i = 0; i < maxFramesInFlight; i++) {

                Fox::Vulkan::Buffer<Fox::Vulkan::PerFrameConstantBuffer>* uniformBuffer = new Fox::Vulkan::Buffer<Fox::Vulkan::PerFrameConstantBuffer>();
                uniformBuffer->Create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                uniformBuffer->Map();
                perFrame[i] = uniformBuffer;

            }
        }
		
		void ConstantBuffers::SyncPerFrame(uint32_t currentImage) {

            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();
            float aspectRatio = renderer->swapchain->GetExtent().width / (float) renderer->swapchain->GetExtent().height;

            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            Fox::Vulkan::PerFrameConstantBuffer perFrame{};
            perFrame.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            perFrame.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            perFrame.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
            perFrame.proj[1][1] *= -1;


            this->perFrame[currentImage]->Update(perFrame);

        }	

        void ConstantBuffers::DeleteBuffers() {

            for (size_t i = 0u; i < perFrame.size(); i++) {
                delete perFrame[i];
            }
        }
		
	}
}