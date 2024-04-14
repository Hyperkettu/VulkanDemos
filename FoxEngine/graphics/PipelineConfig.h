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


			std::string name;
			std::vector<Fox::Vulkan::ShaderConfig> shaders;
			std::vector<Fox::Vulkan::DynamicState> dynamicStates;
			Fox::Vulkan::PrimitiveTopology topology;
			bool primitiveRestartEnable;
		};

	}
}