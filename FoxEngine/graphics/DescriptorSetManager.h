#pragma once

namespace Fox {

	namespace Vulkan {
		class DescriptorSetManager {
		public:

			DescriptorSetManager() = default;
			DescriptorSetManager(uint32_t numFramesInFlight);
			~DescriptorSetManager();
			void CreateDescriptorPools();
			void CreateDescriptorSetLayouts();
			void CreateDescriptorSets();

			VkDescriptorSet* GetAddressOfDescriptorSet(uint32_t imageIndex) {
				return &descriptorSets[imageIndex];
			}

			VkDescriptorSetLayout* GetAddressOfDescriptorSetLayout() {
				return &descriptorSetLayout;
			}

		private: 
			uint32_t numFramesInFlight;
			VkDescriptorPool descriptorPool;
			VkDescriptorSetLayout descriptorSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			
		};

	}

}