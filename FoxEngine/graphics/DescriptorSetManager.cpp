#include "pch.h"

namespace Fox {

	namespace Vulkan {

        DescriptorSetManager::DescriptorSetManager(uint32_t numFramesInFlight) : numFramesInFlight(numFramesInFlight) {
            
        }

        DescriptorSetManager::~DescriptorSetManager() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkDestroyDescriptorPool(device, descriptorPool, nullptr);
            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        
        }
	
		void DescriptorSetManager::CreateDescriptorPools() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            std::array<VkDescriptorPoolSize, 3> poolSizes{};
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = static_cast<uint32_t>(numFramesInFlight);
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[1].descriptorCount = static_cast<uint32_t>(numFramesInFlight);
            poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[2].descriptorCount = static_cast<uint32_t>(numFramesInFlight);

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(numFramesInFlight);

            if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create descriptor pool!");
            }
		}

        void DescriptorSetManager::CreateDescriptorSetLayouts() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

            VkDescriptorSetLayoutBinding ubo2LayoutBinding{};
            ubo2LayoutBinding.binding = 1;
            ubo2LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo2LayoutBinding.descriptorCount = 1;
            ubo2LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            ubo2LayoutBinding.pImmutableSamplers = nullptr; // Optional

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 2;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, ubo2LayoutBinding, samplerLayoutBinding };
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create descriptor set layout!");
            }
        }

        void DescriptorSetManager::CreateDescriptorSets() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

            std::vector<VkDescriptorSetLayout> layouts(numFramesInFlight, descriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(numFramesInFlight);
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(numFramesInFlight);
            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (uint32_t i = 0; i < numFramesInFlight; i++) {
                std::array<VkDescriptorBufferInfo, 1> bufferInfos{};
                bufferInfos[0].buffer = renderer->GetConstantBuffers()->GetPerFrameConstantBuffer(i);
                bufferInfos[0].offset = 0;
                bufferInfos[0].range = sizeof(Fox::Vulkan::PerFrameConstantBuffer);
                
                std::array<VkDescriptorBufferInfo, 1> bufferInfos2{};
                bufferInfos2[0].buffer = renderer->GetConstantBuffers()->GetPerObjectConstantBuffer(i);
                bufferInfos2[0].offset = 0;
                bufferInfos2[0].range = sizeof(Fox::Vulkan::PerObjectConstantBuffer);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = renderer->GetTextureManager()->GetImageViewForTexture();
                imageInfo.sampler = renderer->GetSamplerManager()->GetSampler();

                std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = descriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = bufferInfos.data();

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = descriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pBufferInfo = bufferInfos2.data();

                descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[2].dstSet = descriptorSets[i];
                descriptorWrites[2].dstBinding = 2;
                descriptorWrites[2].dstArrayElement = 0;
                descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[2].descriptorCount = 1;
                descriptorWrites[2].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            }
        }

	}

}