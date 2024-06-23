#pragma once

namespace Fox {

	namespace Vulkan {
		class ConstantBuffers {
		
		public:

			ConstantBuffers() = default;
			~ConstantBuffers();

			inline VkBuffer GetPerFrameConstantBuffer(uint32_t imageIndex) {
				return perFrame[imageIndex]->GetBuffer();
			}

			inline VkBuffer GetPerObjectConstantBuffer(uint32_t imageIndex) {
				return perObject[imageIndex]->GetBuffer();
			}

			void CreateUniformBuffers(uint32_t maxFramesInFlight);
			void SyncPerFrame(uint32_t currentFrame);
			void SyncPerObject(uint32_t currentFrame, const Batch& batch);

		private:

			void DeleteBuffers();

			std::vector<Fox::Vulkan::Buffer<PerFrameConstantBuffer>*> perFrame;
			std::vector<Fox::Vulkan::Buffer<PerObjectConstantBuffer>*> perObject;


		};
	}
}