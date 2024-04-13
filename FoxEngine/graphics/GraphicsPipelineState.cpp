#include "pch.h"

namespace Fox {

	namespace Vulkan {

        GraphicsPipelineState::~GraphicsPipelineState() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            vkDestroyPipeline(device, graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        }

        template<class VertexType>
        void GraphicsPipelineState::Create() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1; // Optional
            pipelineLayoutInfo.pSetLayouts = renderer->GetDesciptorManager()->GetAddressOfDescriptorSetLayout(); // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create pipeline layout!");
            }

            auto bindings = VertexType::getBindingDescription();
            VkPipelineVertexInputStateCreateInfo vertexInputState;
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputState.vertexBindingDescriptionCount = 1;
            vertexInputState.pVertexBindingDescriptions = &bindings; // Optional
            vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexType::getAttributeDescriptions().size());
            vertexInputState.pVertexAttributeDescriptions = VertexType::getAttributeDescriptions().data(); // Optional
            vertexInputState.flags = 0;
            vertexInputState.pNext = nullptr; 

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = &vertexInputState;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil; // Optional
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderer->GetRenderPassManager()->GetRenderPass();
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create graphics pipeline!");
            }

            for (auto shaderModule : shaderModules) {
                vkDestroyShaderModule(device, shaderModule, nullptr);
            }

            shaderModules.clear();
        }

        GraphicsPipelineState& GraphicsPipelineState::WithShader(const std::string& shaderPath, VkShaderStageFlagBits shaderStage) {
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();
            auto shaderByteCode = Fox::Core::FileSystem::ReadBinaryFile(shaderPath);
            VkShaderModule shaderModule = renderer->CreateShaderModule(shaderByteCode);

            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = shaderStage;
            shaderStageInfo.module = shaderModule;
            shaderStageInfo.pName = "main";

            shaderStages.push_back(shaderStageInfo);
            shaderModules.push_back(shaderModule);

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithDynamicState(std::vector<VkDynamicState>& dynamicStates) {
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithInputAssembly(VkPrimitiveTopology primitiveTopology, VkBool32 primitiveRestartEnable) {
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = primitiveTopology;
            inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
            
            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithViewportState(float viewportX, float viewportY, float viewportWidth, float viewportHeight,
            float viewportMinDepth, float viewportMaxDepth, int scissorX, int scissorY, VkExtent2D scissorExtent) {
            VkViewport viewport{};
            viewport.x = viewportX;
            viewport.y = viewportY;
            viewport.width = viewportWidth;
            viewport.height = viewportHeight;
            viewport.minDepth = viewportMinDepth;
            viewport.maxDepth = viewportMaxDepth;

            VkRect2D scissor{};
            scissor.offset = { scissorX, scissorY };
            scissor.extent = scissorExtent;

            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.pScissors = &scissor;

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithRasterizationState(
            VkBool32 depthClampEnable, 
            VkBool32 rasterizerDiscardEnable, 
            VkPolygonMode polygonMode, 
            float lineWidth, 
            VkCullModeFlags cullMode,
            VkFrontFace frontFace,
            VkBool32 depthBiasEnable,
            float depthbiasConstantFactor,
            float depthBiasClamp,
            float depthBiasSlopeFactor) {
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = depthClampEnable;
            rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnable;
            rasterizer.polygonMode = polygonMode;
            rasterizer.lineWidth = lineWidth;
            rasterizer.cullMode = cullMode;
            rasterizer.frontFace = frontFace;

            rasterizer.depthBiasEnable = depthBiasEnable;
            rasterizer.depthBiasConstantFactor = depthbiasConstantFactor; // Optional
            rasterizer.depthBiasClamp = depthBiasClamp; // Optional
            rasterizer.depthBiasSlopeFactor = depthBiasSlopeFactor; // Optional

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithMultisampling(
            VkBool32 sampleShadingEnable, 
            VkSampleCountFlagBits rasterizationSamples,
            float minSampleShading, 
            VkSampleMask* sampleMask, 
            VkBool32 alphaToCoverageEnable, 
            VkBool32 alphaToOneEnable) {
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = sampleShadingEnable;
            multisampling.rasterizationSamples = rasterizationSamples;
            multisampling.minSampleShading = minSampleShading; // Optional
            multisampling.pSampleMask = sampleMask; // Optional
            multisampling.alphaToCoverageEnable = alphaToCoverageEnable; // Optional
            multisampling.alphaToOneEnable = alphaToOneEnable; // Optional

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithColorBlendAttachment(
            VkColorComponentFlags colorWriteMask,
            VkBool32 blendEnable,
            VkBlendFactor srcColorBlendFactor,
            VkBlendFactor dstcColorBlendFactor,
            VkBlendOp colorBlendOp,
            VkBlendFactor srcAlphaBlendFactor,
            VkBlendFactor dstAlphaBlendFactor,
            VkBlendOp alphaBlendOp) {

            colorBlendAttachment.colorWriteMask = colorWriteMask;
            colorBlendAttachment.blendEnable = blendEnable;
            colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor; // Optional
            colorBlendAttachment.dstColorBlendFactor = dstcColorBlendFactor; // Optional
            colorBlendAttachment.colorBlendOp = colorBlendOp; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor; // Optional
            colorBlendAttachment.alphaBlendOp = alphaBlendOp; // Optional

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithColorBlending(
          VkBool32 logicOpEnable, 
          VkLogicOp logicOp, 
          uint32_t attachmentCount,
         float blendConstants[4]
        ) {
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            return *this;
        }

        GraphicsPipelineState& GraphicsPipelineState::WithDepthStencil(
            VkBool32 depthTestEnable,
            VkBool32 depthWriteEnable,
            VkCompareOp depthCompareOp,
            VkBool32 depthBoundsTestEnable,
            float minDepthBounds,
            float maxDepthBounds,
            VkBool32 stencilTestEnable,
            VkStencilOpState frontState,
            VkStencilOpState backState
        ) {
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = depthTestEnable;
            depthStencil.depthWriteEnable = depthWriteEnable;
            depthStencil.depthCompareOp = depthCompareOp;
            depthStencil.depthBoundsTestEnable = depthBoundsTestEnable;
            depthStencil.minDepthBounds = minDepthBounds; // Optional
            depthStencil.maxDepthBounds = maxDepthBounds; // Optional
            depthStencil.stencilTestEnable = stencilTestEnable;
            depthStencil.front = frontState; // Optional
            depthStencil.back = backState; // Optional

            return *this;
        }

		GraphicsPipelineStateManager::~GraphicsPipelineStateManager() {
		}

        std::vector<Fox::Vulkan::PipelineConfig> GraphicsPipelineStateManager::ReadPipelineConfigs(const std::string& path) {
            std::vector<Fox::Vulkan::PipelineConfig> pipelineConfigs;
            std::string data = Fox::Core::FileSystem::ReadFile(path);
            Fox::Core::Json::JSON json;
            json.Parse(data);

            Fox::Core::Json::JSONObject& root = json.Get<Fox::Core::Json::JSONObject>();
            Fox::Core::Json::IntValue& numPipelines = root.Get<Fox::Core::Json::IntValue>("numberOfPipelines");
            uint32_t numberOfPipelines = numPipelines.GetValue();

            Fox::Core::Json::JSONValueArray& pipelinesArray = root.Get<Fox::Core::Json::JSONValueArray>("pipelines");

            for (size_t i = 0u; i < numberOfPipelines; i++) {
                Fox::Core::Json::JSONObject& pipelineData = pipelinesArray.Get<Fox::Core::Json::JSONObject>(i);
                Fox::Core::Json::StringValue& pipelineType = pipelineData.Get<Fox::Core::Json::StringValue>("type");
                Fox::Core::Json::StringValue& pipelinePath = pipelineData.Get<Fox::Core::Json::StringValue>("path");
                Fox::Core::Json::StringValue& pipelineName = pipelineData.Get<Fox::Core::Json::StringValue>("name");


                Fox::Vulkan::PipelineConfig config;
                config.name = pipelineName.value;
                config.ReadFromFile(pipelinePath.value);
                pipelineConfigs.push_back(config);
            }
        
            return pipelineConfigs;
        }


        void GraphicsPipelineStateManager::CreateGraphicsPipelines() {
            Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

            std::vector<Fox::Vulkan::PipelineConfig> pipelineConfigs = ReadPipelineConfigs("pipelines/pipelines.json");

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

            for (size_t i = 0u; i < pipelineConfigs.size(); i++) {
                std::string name = pipelineConfigs[i].name;
                pipelineStates[name] = std::make_shared<GraphicsPipelineState>();
                Fox::Vulkan::GraphicsPipelineState* currentPipeline = pipelineStates[name].get();
                currentPipeline->SetName(name);

                for (size_t shaderIndex = 0u; shaderIndex < pipelineConfigs[i].shaders.size(); shaderIndex++) {
                    *currentPipeline = currentPipeline->WithShader(pipelineConfigs[i].shaders[shaderIndex].path, Fox::Vulkan::ShaderConfig::ToVulkanShader(pipelineConfigs[i].shaders[shaderIndex].shaderType));
                }

                currentPipeline->
                    WithDynamicState(dynamicStates).
                    WithInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE).
                    WithViewportState(0.0f, 0.0f, static_cast<float>(renderer->swapchain->GetExtent().width), static_cast<float>(renderer->swapchain->GetExtent().height),
                        0.0f, 1.0f, 0, 0, renderer->swapchain->GetExtent()).
                    WithRasterizationState(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, 1.0f, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f).
                    WithMultisampling(VK_TRUE, renderer->GetConfig().msaaSamples, 1.0f, nullptr, VK_FALSE, VK_FALSE).
                    WithColorBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO,
                        VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD).
                    WithColorBlending(VK_FALSE, VK_LOGIC_OP_COPY, 1, blendConstants).
                    WithDepthStencil(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, 0.0f, 1.0f, VK_FALSE, {}, {}).
                    Create<Fox::Vulkan::Vertex>();
            }

            SetCurrentPipelineState("default");

        }
	}
}