#pragma once

namespace Fox {

	namespace Vulkan {
	
		class Synchronization {
		public: 

			enum SemaphoreType {
				IMAGE_AVAILABLE = 0,
				RENDER_FINISHED
			};

			Synchronization() = default;
			Synchronization(uint32_t numFramesInFlight);
			~Synchronization();

			void WaitForFence(VkDevice device, uint32_t imageIndex);
			void ResetFence(VkDevice device, uint32_t imageIndex);
			VkFence GetInFlightFence(uint32_t imageIndex);
			VkSemaphore GetSempahore(SemaphoreType semaphoreType, uint32_t imageIndex);

		private: 

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;
		};
	}
}