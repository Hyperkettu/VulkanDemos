#include "pch.h"

namespace Fox {

	namespace Vulkan {

		Synchronization::Synchronization(uint32_t numFramesInFlight) {
			VkDevice device = Fox::Vulkan::Renderer::GetDevice();
			imageAvailableSemaphores.resize(numFramesInFlight);
			renderFinishedSemaphores.resize(numFramesInFlight);
			inFlightFences.resize(numFramesInFlight);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i = 0; i < numFramesInFlight; i++) {

				if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
					vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
					throw std::runtime_error("Failed to create semaphores!");
				}
			}

		}

		Synchronization::~Synchronization() {
			VkDevice device = Fox::Vulkan::Renderer::GetDevice();
			for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
				vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
				vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}
		}

		void Synchronization::WaitForFence(VkDevice device, uint32_t imageIndex) {
			vkWaitForFences(device, 1, &inFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
		}

		void Synchronization::ResetFence(VkDevice device, uint32_t imageIndex) {
			vkResetFences(device, 1, &inFlightFences[imageIndex]);
		}

		VkSemaphore Synchronization::GetSempahore(SemaphoreType semaphoreType, uint32_t imageIndex) {
			switch (semaphoreType)
			{
			case Fox::Vulkan::Synchronization::IMAGE_AVAILABLE:
				return imageAvailableSemaphores[imageIndex];
			case Fox::Vulkan::Synchronization::RENDER_FINISHED:
				return renderFinishedSemaphores[imageIndex];

			default:
				return VK_NULL_HANDLE;
			}
		}

		VkFence Synchronization::GetInFlightFence(uint32_t imageIndex) {
			return inFlightFences[imageIndex];
		}
	}
}