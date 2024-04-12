#pragma once

namespace Fox {

	namespace Vulkan {

		class GraphicsPipelineState {
		public: 

			GraphicsPipelineState() = default;
			~GraphicsPipelineState();

			GraphicsPipelineState& WithShader(const std::string& shaderPath, VkShaderStageFlagBits shaderStage);
			GraphicsPipelineState& WithDynamicState(std::vector<VkDynamicState>& dynamicStates);
			GraphicsPipelineState& WithInputAssembly(VkPrimitiveTopology primitiveTopology, VkBool32 primitiveRestartEnable);
			GraphicsPipelineState& WithViewportState(float viewportX, float viewportY, float viewportWidth, float viewportHeight,
				float viewportMinDepth, float viewportMaxDepth, int scissorX, int scissorY, VkExtent2D scissorExtent);
			GraphicsPipelineState& WithRasterizationState(
				VkBool32 depthClampEnable,
				VkBool32 rasterizerDiscardEnable,
				VkPolygonMode polygonMode,
				float lineWidth,
				VkCullModeFlags cullMode,
				VkFrontFace frontFace,
				VkBool32 depthBiasEnable,
				float depthbiasConstantFactor,
				float depthBiasClamp,
				float depthBiasSlopeFactor);
			GraphicsPipelineState& WithMultisampling(
				VkBool32 sampleShadingEnable,
				VkSampleCountFlagBits rasterizationSamples,
				float minSampleShading,
				VkSampleMask* sampleMask,
				VkBool32 alphaToCoverageEnable,
				VkBool32 alphaToOneEnable);
			GraphicsPipelineState& WithColorBlendAttachment(VkColorComponentFlags colorWriteMask,
				VkBool32 blendEnable,
				VkBlendFactor srcColorBlendFactor,
				VkBlendFactor dstcColorBlendFactor,
				VkBlendOp colorBlendOp,
				VkBlendFactor srcAlphaBlendFactor,
				VkBlendFactor dstAlphaBlendFactor,
				VkBlendOp alphaBlendOp
			);
			GraphicsPipelineState& WithColorBlending(VkBool32 logicOpEnable, VkLogicOp logicOp, uint32_t attachmentCount, float blendConstants[4]);
			GraphicsPipelineState& WithDepthStencil(
				VkBool32 depthTestEnable,
				VkBool32 depthWriteEnable,
				VkCompareOp depthCompareOp,
				VkBool32 depthBoundsTestEnable,
				float minDepthBounds,
				float maxDepthBounds,
				VkBool32 stencilTestEnable,
				VkStencilOpState frontState,
				VkStencilOpState backState
			);

			template<class VertexType>
			void Create();

			inline VkPipeline GetPipelineState() {
				return graphicsPipeline;
			}

			inline VkPipelineLayout GetPipelineLayout() {
				return pipelineLayout;
			}

		private:
			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;

			VkPipelineDynamicStateCreateInfo dynamicState;
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			VkPipelineViewportStateCreateInfo viewportState = {};
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			VkPipelineDepthStencilStateCreateInfo depthStencil{};

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			std::vector<VkShaderModule> shaderModules;
		};
	
		class GraphicsPipelineStateManager {
		public: 
			GraphicsPipelineStateManager() = default;
			~GraphicsPipelineStateManager ();

			inline VkPipeline GetCurrentPipelineState() {
				return currentPipelineState->GetPipelineState();
			}

			void SetAsCurrentPipelineState(std::shared_ptr<GraphicsPipelineState>& pipeline) {
				currentPipelineState = pipeline;
			}

			inline VkPipelineLayout GetCurrentPipelineStateLayout() {
				return currentPipelineState->GetPipelineLayout();
			}

			void Create();

			void CreateGraphicsPipelines();

		private:

			std::shared_ptr<GraphicsPipelineState> graphicsPipelineState;
			std::shared_ptr<GraphicsPipelineState> currentPipelineState;
		};
		
	}
}