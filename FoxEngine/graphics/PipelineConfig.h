#pragma once

namespace Fox {

	namespace Vulkan {

		enum ShaderType {
			VEXTEX,
			FRAGMENT,
			NUM_SHADER_TYPES
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

			std::string name;
			std::vector<Fox::Vulkan::ShaderConfig> shaders;
		};

	}
}