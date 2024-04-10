#pragma once

namespace Fox {

	namespace Vulkan {
		
		struct RendererConfig {
			void* windowHandle;
			VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		};
	}
}