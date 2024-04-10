#pragma once

namespace Fox {

	namespace Vulkan {

		class SamplerManager {
		public:
			SamplerManager() = default;
			SamplerManager(uint32_t maxMipLevels);
			~SamplerManager();

			void CreateSamplers();

			VkSampler GetSampler() {
				return textureSampler;
			}

		private:
			uint32_t maxMipLevels;
			VkSampler textureSampler;

			
		};
	
	}
}