#include "pch.h"

namespace Fox {

	namespace Vulkan {

		Fox::Vulkan::ShaderType ShaderConfig::GetShaderType(std::string& type) {
			if (type == "vertex") {
				return Fox::Vulkan::ShaderType::VEXTEX;
			} else if (type == "fragment") {
				return Fox::Vulkan::ShaderType::FRAGMENT;
			}
			return Fox::Vulkan::ShaderType::NUM_SHADER_TYPES;
		}

		VkShaderStageFlagBits ShaderConfig::ToVulkanShader(Fox::Vulkan::ShaderType type) {
			switch (type)
			{
			case Fox::Vulkan::ShaderType::VEXTEX: return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			case Fox::Vulkan::ShaderType::FRAGMENT: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

			default:
				return VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			}
		}

		Fox::Vulkan::DynamicState PipelineConfig::GetDynamicState(const std::string& state) {
			if (state == "viewport") {
				return Fox::Vulkan::DynamicState::VIEWPORT;
			} else if (state == "scissor") {
				return Fox::Vulkan::DynamicState::SCISSOR;
			}
			return Fox::Vulkan::DynamicState::NUM_DYNAMIC_STATES;
		}

		Fox::Vulkan::PrimitiveTopology PipelineConfig::GetPrimitiveTopology(const std::string& topology) {
			if (topology == "triangle") {
				return Fox::Vulkan::PrimitiveTopology::TRIANGLES;
			} else if (topology == "triangle_strip") {
				return Fox::Vulkan::PrimitiveTopology::TRIANGLE_STRIP;
			} else if (topology == "triangle_fan") {
				return Fox::Vulkan::PrimitiveTopology::TRIANGLE_FAN;
			} else if (topology == "line") {
				return Fox::Vulkan::PrimitiveTopology::LINES;
			} else if (topology == "line_strip") {
				return Fox::Vulkan::PrimitiveTopology::LINE_STRIP;
			} else if (topology == "points") {
				return Fox::Vulkan::PrimitiveTopology::POINTS;
			}
			return Fox::Vulkan::PrimitiveTopology::NUM_PRIMITIVE_TOPOLOGIES;
		}
		
		PipelineConfig::~PipelineConfig() {
		}

		void PipelineConfig::ReadFromFile(const std::string& path) {
			std::string data = Fox::Core::FileSystem::ReadFile(path);
			Fox::Core::Json::JSON json;
			json.Parse(data);

			Fox::Core::Json::JSONObject& root = json.Get<Fox::Core::Json::JSONObject>();
			Fox::Core::Json::IntValue& numShaders = root.Get<Fox::Core::Json::IntValue>("numberOfShaderStages");
			uint32_t numberOfShaderStages = numShaders.GetValue();

			Fox::Core::Json::JSONValueArray& shadersArray = root.Get<Fox::Core::Json::JSONValueArray>("shaders");

			for (size_t i = 0u; i < numberOfShaderStages; i++) {
				Fox::Core::Json::JSONObject& shaderData = shadersArray.Get<Fox::Core::Json::JSONObject>(i);
				Fox::Core::Json::StringValue& shaderType = shaderData.Get<Fox::Core::Json::StringValue>("type");
				Fox::Core::Json::StringValue& shaderPath = shaderData.Get<Fox::Core::Json::StringValue>("path");

				shaders.push_back({ Fox::Vulkan::ShaderConfig::GetShaderType(shaderType.value), shaderPath.value });
			}
			Fox::Core::Json::IntValue& numDynamicStates = root.Get<Fox::Core::Json::IntValue>("numberOfDynamicStates");
			uint32_t numberOfDynamicStates = numDynamicStates.GetValue();
			Fox::Core::Json::JSONValueArray& dynamicStatesArray = root.Get<Fox::Core::Json::JSONValueArray>("dynamicStates");

			for (size_t i = 0u; i < numberOfDynamicStates; i++) {
				Fox::Core::Json::StringValue& dynamicState = dynamicStatesArray.Get<Fox::Core::Json::StringValue>(i);
				dynamicStates.push_back(Fox::Vulkan::PipelineConfig::GetDynamicState(dynamicState.value));
			}

			Fox::Core::Json::JSONObject& inputAssemby = root.Get<Fox::Core::Json::JSONObject>("inputAssembly");
			Fox::Core::Json::StringValue& primitiveTopologyString = inputAssemby.Get<Fox::Core::Json::StringValue>("primitiveTopology");
			topology = Fox::Vulkan::PipelineConfig::GetPrimitiveTopology(primitiveTopologyString.value);

			Fox::Core::Json::BoolValue& primitiveRestartEnableBool = inputAssemby.Get<Fox::Core::Json::BoolValue>("primitiveRestartEnable");
			primitiveRestartEnable = primitiveRestartEnableBool.GetValue();
			


		}
	}
}


