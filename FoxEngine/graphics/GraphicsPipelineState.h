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

			void SetName(const std::string& name) {
				this->name = name;
			}

			inline std::string GetName() const {
				return name;
			}

			inline void SetConfig(const Fox::Vulkan::PipelineConfig& config) {
				this->config = config;
			}

			inline Fox::Vulkan::PipelineConfig& GetConfig() {
				return config;
			}

			static VkPrimitiveTopology GetVulkanPrimitiveTopology(Fox::Vulkan::PrimitiveTopology topology);
			static VkPolygonMode GetVulkanPolygonMode(Fox::Vulkan::PolygonMode polygonMode);
			static VkCullModeFlagBits GetVulkanCullMode(Fox::Vulkan::CullMode cullMode);
			static VkFrontFace GetVulkanFrontFace(Fox::Vulkan::FrontFace frontFace);
			static VkBlendFactor GetVulkanBlendFactor(Fox::Vulkan::BlendFactor blendFactor) {
				return static_cast<VkBlendFactor>(blendFactor);
			}
			static VkBlendOp GetVulkanBlendOp(Fox::Vulkan::BlendOperation blendOp) {
				return static_cast<VkBlendOp>(blendOp);
			}
			static VkLogicOp GetVulkanLogicOp(Fox::Vulkan::LogicOperation logicOp) {
				return static_cast<VkLogicOp>(logicOp);
			}

		private:
			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;

			std::string name;

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

			Fox::Vulkan::PipelineConfig config;
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

			void SetCurrentPipelineState(const std::string& pipelineName) {
				SetAsCurrentPipelineState(pipelineStates[pipelineName]);
			}

			inline VkPipelineLayout GetCurrentPipelineStateLayout() {
				return currentPipelineState->GetPipelineLayout();
			}

			void Create();
			std::vector<Fox::Vulkan::PipelineConfig> ReadPipelineConfigs(const std::string& path);

			void CreateGraphicsPipelines();

			float GetCurrentLineWidth() {
				return currentPipelineState->GetConfig().lineWidth;
			}

			bool RenderWideLines() {
				return currentPipelineState->GetConfig().lineWidth != 1.0f && (currentPipelineState->GetConfig().topology == Fox::Vulkan::PrimitiveTopology::LINES ||
					currentPipelineState->GetConfig().topology == Fox::Vulkan::PrimitiveTopology::LINE_STRIP);
			}

		private:

			std::shared_ptr<GraphicsPipelineState> currentPipelineState;
			std::unordered_map<std::string, std::shared_ptr<Fox::Vulkan::GraphicsPipelineState>> pipelineStates;
		};
		
	}
}