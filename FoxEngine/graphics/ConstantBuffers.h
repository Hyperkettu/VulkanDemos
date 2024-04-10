#pragma once

namespace Fox {

	namespace Vulkan {
		class ConstantBuffers {
		
		public:

			ConstantBuffers() = default;
			~ConstantBuffers();

			inline VkBuffer GetPerFrameConstantBuffer(uint32_t imageIndex) {
				return uniformBuffers[imageIndex]->GetBuffer();
			}

			void CreateUniformBuffers(uint32_t maxFramesInFlight);
			void SyncPerFrame(uint32_t currentFrame);

		private:

			void DeleteBuffers();

			std::vector<Fox::Vulkan::Buffer<PerFrameConstantBuffer>*> uniformBuffers;


		};
	}
}