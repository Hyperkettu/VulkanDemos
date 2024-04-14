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

        VkPrimitiveTopology GraphicsPipelineState::GetVulkanPrimitiveTopology(Fox::Vulkan::PrimitiveTopology topology) {
            switch (topology)
            {
            case Fox::Vulkan::PrimitiveTopology::TRIANGLES:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    
            case Fox::Vulkan::PrimitiveTopology::TRIANGLE_STRIP:
                    return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case Fox::Vulkan::PrimitiveTopology::TRIANGLE_FAN:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case Fox::Vulkan::PrimitiveTopology::LINES:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case Fox::Vulkan::PrimitiveTopology::LINE_STRIP:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case Fox::Vulkan::PrimitiveTopology::POINTS:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            default:
                return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            }
        }

        VkPolygonMode GraphicsPipelineState::GetVulkanPolygonMode(Fox::Vulkan::PolygonMode polygonMode) {
            switch (polygonMode)
            {
            case Fox::Vulkan::PolygonMode::FILL:
                return VkPolygonMode::VK_POLYGON_MODE_FILL;
            case Fox::Vulkan::PolygonMode::WIREFRAME:
                return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case Fox::Vulkan::PolygonMode::POINT:
                return VkPolygonMode::VK_POLYGON_MODE_POINT;
            default:
                return VkPolygonMode::VK_POLYGON_MODE_FILL;
            }
        }

        VkCullModeFlagBits GraphicsPipelineState::GetVulkanCullMode(Fox::Vulkan::CullMode cullMode) {
            switch (cullMode)
            {
            case Fox::Vulkan::FRONT:
                return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            case Fox::Vulkan::BACK:
                return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            case Fox::Vulkan::FRONT_AND_BACK:
                return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
            default:
                return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            }
        }

        VkFrontFace GraphicsPipelineState::GetVulkanFrontFace(Fox::Vulkan::FrontFace frontFace) {
            switch (frontFace)
            {
            case Fox::Vulkan::CLOCKWISE:
                return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            case Fox::Vulkan::COUNTER_CLOCKWISE:
                return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default:
                return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            }
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

            float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

            for (size_t i = 0u; i < pipelineConfigs.size(); i++) {
                std::string name = pipelineConfigs[i].name;
                pipelineStates[name] = std::make_shared<GraphicsPipelineState>();
                Fox::Vulkan::GraphicsPipelineState* currentPipeline = pipelineStates[name].get();
                currentPipeline->SetName(name);
                currentPipeline->SetConfig(pipelineConfigs[i]);

                for (size_t shaderIndex = 0u; shaderIndex < pipelineConfigs[i].shaders.size(); shaderIndex++) {
                    if (pipelineConfigs[i].rasterizerDiscardEnable && pipelineConfigs[i].shaders[shaderIndex].shaderType == Fox::Vulkan::ShaderType::FRAGMENT) {
                        std::cout << "Warning: RasterizerDiscard in enabled but pipelineConfig still has fragment shader set in json. Skipping fragment shader." << std::endl;
                        continue;
                    }
                    *currentPipeline = currentPipeline->WithShader(pipelineConfigs[i].shaders[shaderIndex].path, Fox::Vulkan::ShaderConfig::ToVulkanShader(pipelineConfigs[i].shaders[shaderIndex].shaderType));
                }

                std::vector<VkDynamicState> dynamicStates;
                for (size_t j = 0u; j < pipelineConfigs[i].dynamicStates.size(); j++) {
                    if (pipelineConfigs[i].dynamicStates[j] == Fox::Vulkan::DynamicState::VIEWPORT) {
                        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
                    } else if (pipelineConfigs[i].dynamicStates[j] == Fox::Vulkan::DynamicState::SCISSOR) {
                        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
                    }
                }

                float lineWidth = pipelineConfigs[i].lineWidth;

                if (lineWidth != 1.0f) {
                    dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
                }

                VkPrimitiveTopology primitiveTopology = Fox::Vulkan::GraphicsPipelineState::GetVulkanPrimitiveTopology(pipelineConfigs[i].topology);
                VkPolygonMode polygonMode = Fox::Vulkan::GraphicsPipelineState::GetVulkanPolygonMode(pipelineConfigs[i].polygonMode);
                VkCullModeFlagBits cullMode = Fox::Vulkan::GraphicsPipelineState::GetVulkanCullMode(pipelineConfigs[i].cullMode);
                VkFrontFace frontFace = Fox::Vulkan::GraphicsPipelineState::GetVulkanFrontFace(pipelineConfigs[i].frontFace);

                VkSampleCountFlagBits msaaSamples = Fox::Vulkan::PipelineConfig::ToVulkanMultiSamples(pipelineConfigs[i].msaaSamples);
                if (static_cast<uint32_t>(msaaSamples) > static_cast<uint32_t>(renderer->GetConfig().msaaSamples)) {
                    std::cout << "Warning: pipeline's config JSON has a higher value in MSAA sample count than hardware supports. Downgrading to max MSAA samples." << std::endl;
                    msaaSamples = renderer->GetConfig().msaaSamples;
                } 

                // TODO MSAA 1 support for render passes as well, currently error if msaa is 1

                currentPipeline->
                    WithDynamicState(dynamicStates). 
                    WithInputAssembly(primitiveTopology, pipelineConfigs[i].primitiveRestartEnable ? VK_TRUE : VK_FALSE).
                    WithViewportState(0.0f, 0.0f, static_cast<float>(renderer->swapchain->GetExtent().width), static_cast<float>(renderer->swapchain->GetExtent().height),
                        0.0f, 1.0f, 0, 0, renderer->swapchain->GetExtent()).
                    WithRasterizationState(pipelineConfigs[i].depthClampEnable ? VK_TRUE : VK_FALSE, pipelineConfigs[i].rasterizerDiscardEnable ? VK_TRUE : VK_FALSE, polygonMode, lineWidth,
                        cullMode, frontFace, pipelineConfigs[i].depthBiasEnable ? VK_TRUE : VK_FALSE, pipelineConfigs[i].depthBiasConstantFactor, pipelineConfigs[i].depthBiasClamp, pipelineConfigs[i].depthBiasSlopeFactor).
                    WithMultisampling(pipelineConfigs[i].sampleShadingEnable ? VK_TRUE : VK_FALSE, msaaSamples, pipelineConfigs[i].minSampleShading, nullptr, 
                        pipelineConfigs[i].alphaToCoverageEnable ? VK_TRUE : VK_FALSE, pipelineConfigs[i].alphaToOneEnable ?  VK_TRUE : VK_FALSE).
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