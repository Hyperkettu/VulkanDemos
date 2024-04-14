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

		Fox::Vulkan::PolygonMode PipelineConfig::GetPolygonMode(const std::string& polygonMode) {
			if (polygonMode == "fill") {
				return Fox::Vulkan::PolygonMode::FILL;
			} else if (polygonMode == "wireframe") {
				return Fox::Vulkan::PolygonMode::WIREFRAME;
			} else if (polygonMode == "point") {
				return Fox::Vulkan::PolygonMode::POINT;
			}

			return Fox::Vulkan::PolygonMode::NUM_POLYGON_MODES;
		}

		Fox::Vulkan::CullMode PipelineConfig::GetCullMode(const std::string& cullMode) {
			if (cullMode == "front") {
				return Fox::Vulkan::CullMode::FRONT;
			} else if (cullMode == "back") {
				return Fox::Vulkan::CullMode::BACK;
			} else if (cullMode == "front_and_back") {
				return Fox::Vulkan::CullMode::FRONT_AND_BACK;
			}
			return Fox::Vulkan::CullMode::NUM_CULL_MODES;
		}

		Fox::Vulkan::FrontFace PipelineConfig::GetFrontFace(const std::string& cullMode) {
			if (cullMode == "clockwise") {
				return Fox::Vulkan::FrontFace::CLOCKWISE;
			} else if (cullMode == "counter_clockwise") {
				return Fox::Vulkan::FrontFace::COUNTER_CLOCKWISE;
			}
			return Fox::Vulkan::FrontFace::NUM_FRONT_FACES;
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
			
			Fox::Core::Json::JSONObject& rasterization = root.Get<Fox::Core::Json::JSONObject>("rasterization");
			Fox::Core::Json::BoolValue& depthClampEnableBool = rasterization.Get<Fox::Core::Json::BoolValue>("depthClampEnable");
			depthClampEnable = depthClampEnableBool.GetValue();
			Fox::Core::Json::BoolValue& rasterizerDiscardEnableBool = rasterization.Get<Fox::Core::Json::BoolValue>("rasterizerDiscardEnable");
			rasterizerDiscardEnable = rasterizerDiscardEnableBool.GetValue();
			Fox::Core::Json::StringValue& polygonModeString = rasterization.Get<Fox::Core::Json::StringValue>("polygonMode");
			polygonMode = Fox::Vulkan::PipelineConfig::GetPolygonMode(polygonModeString.value);
			Fox::Core::Json::FloatValue& lineWidthFloat = rasterization.Get<Fox::Core::Json::FloatValue>("lineWidth");
			lineWidth = lineWidthFloat.GetValue();
			Fox::Core::Json::StringValue& cullModeString = rasterization.Get<Fox::Core::Json::StringValue>("cullMode");
			cullMode = Fox::Vulkan::PipelineConfig::GetCullMode(cullModeString.value);

			Fox::Core::Json::StringValue& frontFaceString = rasterization.Get<Fox::Core::Json::StringValue>("frontFace");
			frontFace = Fox::Vulkan::PipelineConfig::GetFrontFace(frontFaceString.value);

			Fox::Core::Json::BoolValue& depthBiasEnableBool = rasterization.Get<Fox::Core::Json::BoolValue>("depthBiasEnable");
			depthBiasEnable = depthBiasEnableBool.GetValue();

			Fox::Core::Json::FloatValue& depthBiasConstantFactorFloat = rasterization.Get<Fox::Core::Json::FloatValue>("depthBiasConstantFactor");
			depthBiasConstantFactor = depthBiasConstantFactorFloat.GetValue();
			Fox::Core::Json::FloatValue& depthBiasClampFloat = rasterization.Get<Fox::Core::Json::FloatValue>("depthBiasClamp");
			depthBiasClamp = depthBiasClampFloat.GetValue();
			Fox::Core::Json::FloatValue& depthBiasSlopeFactorFloat = rasterization.Get<Fox::Core::Json::FloatValue>("depthBiasSlopeFactor");
			depthBiasSlopeFactor = depthBiasSlopeFactorFloat.GetValue();

			// multisampling
			Fox::Core::Json::JSONObject& multisampling = root.Get<Fox::Core::Json::JSONObject>("multisampling");
			Fox::Core::Json::BoolValue& sampleShadingEnableBool = multisampling.Get<Fox::Core::Json::BoolValue>("sampleShadingEnable");
			sampleShadingEnable = sampleShadingEnableBool.GetValue();
			Fox::Core::Json::IntValue& msaaSamplesInt = multisampling.Get<Fox::Core::Json::IntValue>("msaaSamples");
			msaaSamples = msaaSamplesInt.GetValue();
			Fox::Core::Json::FloatValue& minSampleShadingFloat = multisampling.Get<Fox::Core::Json::FloatValue>("minSampleShading");
			minSampleShading = minSampleShadingFloat.GetValue();
			Fox::Core::Json::BoolValue& alphaToCoverageEnableBool = multisampling.Get<Fox::Core::Json::BoolValue>("alphaToCoverageEnable");
			alphaToCoverageEnable = alphaToCoverageEnableBool.GetValue();
			Fox::Core::Json::BoolValue& alphaToOneEnableBool = multisampling.Get<Fox::Core::Json::BoolValue>("alphaToOneEnable");
			alphaToOneEnable = alphaToOneEnableBool.GetValue();

		}
	}
}


