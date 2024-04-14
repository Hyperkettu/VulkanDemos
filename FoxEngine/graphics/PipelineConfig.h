#pragma once

namespace Fox {

	namespace Vulkan {

		enum ShaderType {
			VEXTEX,
			FRAGMENT,
			NUM_SHADER_TYPES
		};

		enum DynamicState {
			VIEWPORT,
			SCISSOR,
			NUM_DYNAMIC_STATES
		};

		enum PrimitiveTopology {
			TRIANGLES,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			LINES,
			LINE_STRIP,
			POINTS,
			NUM_PRIMITIVE_TOPOLOGIES
		};

		enum PolygonMode {
			FILL,
			WIREFRAME,
			POINT,
			NUM_POLYGON_MODES
		};

		enum CullMode {
			FRONT,
			BACK,
			FRONT_AND_BACK,
			NUM_CULL_MODES
		};

		enum FrontFace {
			CLOCKWISE,
			COUNTER_CLOCKWISE,
			NUM_FRONT_FACES
		};

		struct ShaderConfig {
			ShaderType shaderType;
			std::string path;

			static Fox::Vulkan::ShaderType GetShaderType(std::string& type);
			static VkShaderStageFlagBits ToVulkanShader(Fox::Vulkan::ShaderType type);

		};
	
		struct PipelineConfig {

			PipelineConfig() = default;
			~PipelineConfig();

			void ReadFromFile(const std::string& path);
			static Fox::Vulkan::DynamicState GetDynamicState(const std::string& state);
			static Fox::Vulkan::PrimitiveTopology GetPrimitiveTopology(const std::string& topology);
			static Fox::Vulkan::PolygonMode GetPolygonMode(const std::string& polygonMode);
			static Fox::Vulkan::CullMode GetCullMode(const std::string& cullMode);
			static Fox::Vulkan::FrontFace GetFrontFace(const std::string& frontFace);
			static VkSampleCountFlagBits ToVulkanMultiSamples(uint32_t msaaSamples) {
				return static_cast<VkSampleCountFlagBits>(msaaSamples);
			}

			std::string name;

			// shaders stages
			std::vector<Fox::Vulkan::ShaderConfig> shaders;

			// dynamic states
			std::vector<Fox::Vulkan::DynamicState> dynamicStates;

			// input assmbly state
			Fox::Vulkan::PrimitiveTopology topology;
			bool primitiveRestartEnable;

			// rasterization state
			bool depthClampEnable;
			bool rasterizerDiscardEnable;
			Fox::Vulkan::PolygonMode polygonMode;
			float lineWidth;
			Fox::Vulkan::CullMode cullMode;
			Fox::Vulkan::FrontFace frontFace;
			bool depthBiasEnable;
			float depthBiasConstantFactor;
			float depthBiasClamp;
			float depthBiasSlopeFactor;

			// multisampling state
			bool sampleShadingEnable;
			uint32_t msaaSamples;
			float minSampleShading;
			bool alphaToCoverageEnable;
			bool alphaToOneEnable;
		};

	}
}