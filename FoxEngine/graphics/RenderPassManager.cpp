#include "pch.h"

namespace Fox {

	namespace Vulkan {

        RenderPass::~RenderPass() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkDestroyRenderPass(device, renderPass, nullptr);
        }

        RenderPass& RenderPass::WithAttachment(
            VkFormat format, 
            VkSampleCountFlagBits numSamples, 
            VkAttachmentLoadOp colorLoadOp, 
            VkAttachmentStoreOp colorStoreOp, 
            VkAttachmentLoadOp stencilLoadOp, 
            VkAttachmentStoreOp stencilStoreOp,
            VkImageLayout initialLayout, 
            VkImageLayout finalLayout
        ) {
            VkAttachmentDescription attachment{};
            attachment.format = format;
            attachment.samples = numSamples;
            attachment.loadOp = colorLoadOp;
            attachment.storeOp = colorStoreOp;
            attachment.stencilLoadOp = stencilLoadOp;
            attachment.stencilStoreOp = stencilStoreOp;
            attachment.initialLayout = initialLayout;
            attachment.finalLayout = finalLayout;

            VkAttachmentReference attachmentRef{};
            attachmentRef.attachment = static_cast<uint32_t>(attachments.size());
            attachmentRef.layout = finalLayout;

            attachments.push_back(attachment);

            if (finalLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
                colorAttachmentRef.push_back(attachmentRef);
            } else if (finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                depthStencilAttachmentRef.push_back(attachmentRef);
            }

            return *this;
        }

        RenderPass& RenderPass::WithColorAttachmentResolve(
            VkFormat format,
            VkSampleCountFlagBits numSamples,
            VkAttachmentLoadOp colorLoadOp,
            VkAttachmentStoreOp colorStoreOp,
            VkAttachmentLoadOp stencilLoadOp,
            VkAttachmentStoreOp stencilStoreOp,
            VkImageLayout initialLayout,
            VkImageLayout finalLayout) {

            VkAttachmentDescription attachment{};
            attachment.format = format;
            attachment.samples = numSamples;
            attachment.loadOp = colorLoadOp;
            attachment.storeOp = colorStoreOp;
            attachment.stencilLoadOp = stencilLoadOp;
            attachment.stencilStoreOp = stencilStoreOp;
            attachment.initialLayout = initialLayout;
            attachment.finalLayout = finalLayout;

            colorAttachmentResolveRef.attachment = attachments.size();
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachment);

            return *this;
        }

        void RenderPass::Create() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = colorAttachmentRef.size();
            subpass.pColorAttachments = colorAttachmentRef.data();
            subpass.pDepthStencilAttachment = depthStencilAttachmentRef.data();
            subpass.pResolveAttachments = &colorAttachmentResolveRef;
           
            subpasses.push_back(subpass);

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;;
            subpassDependencies.push_back(dependency);

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = subpasses.size();
            renderPassInfo.pSubpasses = subpasses.data();
            renderPassInfo.dependencyCount = subpassDependencies.size();
            renderPassInfo.pDependencies = subpassDependencies.data();

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render pass!");
            }
        }
	
		RenderPassManager::~RenderPassManager() { 
            DestroyRenderPass();
		}

        void RenderPassManager::DestroyRenderPass() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            renderPass = nullptr;
        }

		void RenderPassManager::CreateRenderPass() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

            renderPass = std::make_shared<Fox::Vulkan::RenderPass>();

            renderPass->WithAttachment(
                renderer->swapchain->GetImageFormat(),
                renderer->GetConfig().msaaSamples,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            ).WithAttachment(
                renderer->FindDepthFormat(),
                renderer->GetConfig().msaaSamples,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            ).WithColorAttachmentResolve(
                renderer->swapchain->GetImageFormat(),
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                ).Create();
		}
	
	}
}