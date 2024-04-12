#pragma once

namespace Fox {

	namespace Vulkan {

		class RenderPass {

		public: 
			RenderPass() = default;
			~RenderPass();

			RenderPass& WithAttachment(
				VkFormat format,
				VkSampleCountFlagBits numSamples,
				VkAttachmentLoadOp colorLoadOp,
				VkAttachmentStoreOp colorStoreOp,
				VkAttachmentLoadOp stencilLoadOp,
				VkAttachmentStoreOp stencilStoreOp,
				VkImageLayout initialLayout,
				VkImageLayout finalLayout);
			RenderPass& WithColorAttachmentResolve(
				VkFormat format,
				VkSampleCountFlagBits numSamples,
				VkAttachmentLoadOp colorLoadOp,
				VkAttachmentStoreOp colorStoreOp,
				VkAttachmentLoadOp stencilLoadOp,
				VkAttachmentStoreOp stencilStoreOp,
				VkImageLayout initialLayout,
				VkImageLayout finalLayout);
			void Create();
			VkRenderPass GetPass() {
				return renderPass;
			}

		private:
			std::vector<VkAttachmentDescription> attachments;
			std::vector<VkSubpassDescription> subpasses;
			std::vector<VkSubpassDependency> subpassDependencies;

			std::vector<VkAttachmentReference> colorAttachmentRef;
			std::vector<VkAttachmentReference> depthStencilAttachmentRef;
			VkAttachmentReference colorAttachmentResolveRef;

			VkRenderPass renderPass;
		};
	
		class RenderPassManager {

		public: 
			RenderPassManager() = default;
			~RenderPassManager();

			void CreateRenderPass();
			void DestroyRenderPass();

			inline VkRenderPass GetRenderPass() {
				return renderPass->GetPass();
			}

		private: 

			std::shared_ptr<Fox::Vulkan::RenderPass> renderPass;
		};
	
	}
}