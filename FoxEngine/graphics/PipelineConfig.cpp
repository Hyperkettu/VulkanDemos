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

		Fox::Vulkan::ColorWriteMask PipelineConfig::GetColorWriteMask(std::vector<std::string>& maskArray) {
			Fox::Vulkan::ColorWriteMask mask = { false, false, false, false };
			for (size_t i = 0u; i < maskArray.size(); i++) {
				if (maskArray[i] == "r") {
					mask.red = true;
				} else if (maskArray[i] == "g") {
					mask.green = true;
				} else if (maskArray[i] == "b") {
					mask.blue = true;
				} else if (maskArray[i] == "a") {
					mask.alpha = true;
				}
			}
			return mask;
		}

		Fox::Vulkan::BlendFactor PipelineConfig::GetBlendFactor(const std::string& blendFactor) {
			if (blendFactor == "zero") {
				return Fox::Vulkan::BlendFactor::ZERO;
			} else if (blendFactor == "one") {
				return Fox::Vulkan::BlendFactor::ONE;
			} else if (blendFactor == "src_color") {
				return Fox::Vulkan::BlendFactor::SRC_COLOR;
			} else if (blendFactor == "one_minus_src_color") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_SRC_COLOR;
			} else if (blendFactor == "dst_color") {
				return Fox::Vulkan::BlendFactor::DST_COLOR;
			} else if (blendFactor == "one_minus_dst_color") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_DST_COLOR;
			} else if (blendFactor == "src_alpha") {
				return Fox::Vulkan::BlendFactor::SRC_ALPHA;
			} else if (blendFactor == "one_minus_src_alpha") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_SRC_ALPHA;
			} else if (blendFactor == "dst_alpha") {
				return Fox::Vulkan::BlendFactor::DST_ALPHA;
			} else if (blendFactor == "one_minus_dst_alpha") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_DST_ALPHA;
			} else if (blendFactor == "constant_color") {
				return Fox::Vulkan::BlendFactor::CONSTANT_COLOR;
			} else if (blendFactor == "one_minus_constant_color") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
			} else if (blendFactor == "constant_alpha") {
				return Fox::Vulkan::BlendFactor::CONSTANT_ALPHA;
			} else if (blendFactor == "one_minus_constant_alpha") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
			} else if (blendFactor == "src_alpha_saturate") {
				return Fox::Vulkan::BlendFactor::SRC_ALPHA_SATURATE;
			} else if(blendFactor == "src1_color") {
				return Fox::Vulkan::BlendFactor::SRC1_COLOR;
			} else if (blendFactor == "one_minus_src1_color") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_SRC1_COLOR;
			} else if (blendFactor == "src1_alpha") {
				return Fox::Vulkan::BlendFactor::SRC1_ALPHA;
			} else if (blendFactor == "one_minus_src1_alpha") {
				return Fox::Vulkan::BlendFactor::ONE_MINUS_SRC1_ALPHA;
			}

			return Fox::Vulkan::NUM_BLEND_FACTORS;
		}

		Fox::Vulkan::BlendOperation PipelineConfig::GetBlendOperation(const std::string& blendOp) {
			if (blendOp == "add") {
				return Fox::Vulkan::BlendOperation::ADD;
			} else if (blendOp == "subtract") {
				return Fox::Vulkan::BlendOperation::SUBTRACT;
			} else if (blendOp == "reverse_subtract") {
				return Fox::Vulkan::BlendOperation::REVERSE_SUBTRACT;
			} else if (blendOp == "min") {
				return Fox::Vulkan::BlendOperation::MIN;
			} else if (blendOp == "max") {
				return Fox::Vulkan::BlendOperation::MAX;
			} else if (blendOp == "zero_ext") {
				return Fox::Vulkan::BlendOperation::ZERO_EXT;
			} else if (blendOp == "src_ext") {
				return Fox::Vulkan::BlendOperation::SRC_EXT;
			} else if (blendOp == "dst_ext") {
				return Fox::Vulkan::BlendOperation::DST_EXT;
			} else if (blendOp == "src_over_ext") {
				return Fox::Vulkan::BlendOperation::SRC_OVER_EXT;
			} else if (blendOp == "dst_over_ext") {
				return Fox::Vulkan::BlendOperation::DST_OVER_EXT;
			} else if (blendOp == "src_in_ext") {
				return Fox::Vulkan::BlendOperation::SRC_IN_EXT;
			} else if (blendOp == "dst_in_ext") {
				return Fox::Vulkan::BlendOperation::DST_IN_EXT;
			} else if (blendOp == "src_out_ext") {
				return Fox::Vulkan::BlendOperation::SRC_OUT_EXT;
			} else if (blendOp == "dst_out_ext") {
				return Fox::Vulkan::BlendOperation::DST_OUT_EXT;
			} else if (blendOp == "src_atop_ext") {
				return Fox::Vulkan::BlendOperation::SRC_ATOP_EXT;
			} else if (blendOp == "dst_atop_ext") {
				return Fox::Vulkan::BlendOperation::DST_ATOP_EXT;
			} else if (blendOp == "xor_ext") {
				return Fox::Vulkan::BlendOperation::XOR_EXT;
			} else if (blendOp == "multiply_ext") {
				return Fox::Vulkan::BlendOperation::MULTIPLY_EXT;
			} else if (blendOp == "screen_ext") {
				return Fox::Vulkan::BlendOperation::SCREEN_EXT;
			} else if (blendOp == "overlay_ext") {
				return Fox::Vulkan::BlendOperation::OVERLAY_EXT;
			} else if (blendOp == "darken_ext") {
				return Fox::Vulkan::BlendOperation::DARKEN_EXT;
			} else if (blendOp == "lighten_ext") {
				return Fox::Vulkan::BlendOperation::LIGHTEN_EXT;
			} else if (blendOp == "colordodge_ext") {
				return Fox::Vulkan::BlendOperation::COLORDODGE_EXT;
			} else if (blendOp == "colorburn_ext") {
				return Fox::Vulkan::BlendOperation::COLORBURN_EXT;
			} else if (blendOp == "hardlight_ext") {
				return Fox::Vulkan::BlendOperation::HARDLIGHT_EXT;
			} else if (blendOp == "softlight_ext") {
				return Fox::Vulkan::BlendOperation::SOFTLIGHT_EXT;
			} else if (blendOp == "difference_ext") {
				return Fox::Vulkan::BlendOperation::DIFFERENCE_EXT;
			} else if (blendOp == "exclusion_ext") {
				return Fox::Vulkan::BlendOperation::EXCLUSION_EXT;
			} else if (blendOp == "invert_ext") {
				return Fox::Vulkan::BlendOperation::INVERT_EXT;
			} else if (blendOp == "invert_rgb_ext") {
				return Fox::Vulkan::BlendOperation::INVERT_RGB_EXT;
			} else if (blendOp == "lineardodge_ext") {
				return Fox::Vulkan::BlendOperation::LINEARDODGE_EXT;
			} else if (blendOp == "linearburn_ext") {
				return Fox::Vulkan::BlendOperation::LINEARBURN_EXT;
			} else if (blendOp == "vividlight_ext") {
				return Fox::Vulkan::BlendOperation::VIVIDLIGHT_EXT;
			} else if (blendOp == "linearlight_ext") {
				return Fox::Vulkan::BlendOperation::LINEARLIGHT_EXT;
			} else if (blendOp == "pinlight_ext") {
				return Fox::Vulkan::BlendOperation::PINLIGHT_EXT;
			} else if (blendOp == "hardmix_ext") {
				return Fox::Vulkan::BlendOperation::HARDMIX_EXT;
			} else if (blendOp == "hsl_hue_ext") {
				return Fox::Vulkan::BlendOperation::HSL_HUE_EXT;
			} else if (blendOp == "hsl_saturation_ext") {
				return Fox::Vulkan::BlendOperation::HSL_SATURATION_EXT;
			} else if (blendOp == "hsl_color_ext") {
				return Fox::Vulkan::BlendOperation::HSL_COLOR_EXT;
			} else if (blendOp == "hsl_luminosity_ext") {
				return Fox::Vulkan::BlendOperation::HSL_LUMINOSITY_EXT;
			} else if (blendOp == "plus_ext") {
				return Fox::Vulkan::BlendOperation::PLUS_EXT;
			} else if (blendOp == "plus_clamped_ext") {
				return Fox::Vulkan::BlendOperation::PLUS_CLAMPED_EXT;
			} else if (blendOp == "plus_clamped_alpha_ext") {
				return Fox::Vulkan::BlendOperation::PLUS_CLAMPED_ALPHA_EXT;
			} else if (blendOp == "plus_darker_ext") {
				return Fox::Vulkan::BlendOperation::PLUS_DARKER_EXT;
			} else if (blendOp == "minus_ext") {
				return Fox::Vulkan::BlendOperation::MINUS_EXT;
			} else if (blendOp == "minus_clamped_ext") {
				return Fox::Vulkan::BlendOperation::MINUS_CLAMPED_EXT;
			} else if (blendOp == "contrast_ext") {
				return Fox::Vulkan::BlendOperation::CONTRAST_EXT;
			} else if (blendOp == "invert_ovg_ext") {
				return Fox::Vulkan::BlendOperation::INVERT_OVG_EXT;
			} else if (blendOp == "red_ext") {
				return Fox::Vulkan::BlendOperation::RED_EXT;
			} else if (blendOp == "green_ext") {
				return Fox::Vulkan::BlendOperation::GREEN_EXT;
			} else if (blendOp == "blue_ext") {
				return Fox::Vulkan::BlendOperation::BLUE_EXT;
			}
			return Fox::Vulkan::BlendOperation::NUM_BLEND_OPERATIONS;
		}

		Fox::Vulkan::LogicOperation PipelineConfig::GetLogicOperation(const std::string& logicOp) {
			if (logicOp == "clear") {
				return Fox::Vulkan::LogicOperation::CLEAR;
			} else if (logicOp == "and") {
				return Fox::Vulkan::LogicOperation::AND;
			} else if (logicOp == "and_reverse") {
				return Fox::Vulkan::LogicOperation::AND_REVERSE;
			} else if (logicOp == "copy") {
				return Fox::Vulkan::LogicOperation::COPY;
			} else if (logicOp == "and_inverted") {
				return Fox::Vulkan::LogicOperation::AND_INVERTED;
			} else if (logicOp == "no_op") {
				return Fox::Vulkan::LogicOperation::NO_OP;
			} else if (logicOp == "xor") {
				return Fox::Vulkan::LogicOperation::XOR;
			} else if (logicOp == "or") {
				return Fox::Vulkan::LogicOperation::OR;
			} else if (logicOp == "nor") {
				return Fox::Vulkan::LogicOperation::NOR;
			} else if (logicOp == "equivalent") {
				return Fox::Vulkan::LogicOperation::EQUIVALENT;
			} else if (logicOp == "invert") {
				return Fox::Vulkan::LogicOperation::INVERT;
			} else if (logicOp == "or_reverse") {
				return Fox::Vulkan::LogicOperation::OR_REVERSE;
			} else if (logicOp == "copy_inverted") {
				return Fox::Vulkan::LogicOperation::COPY_INVERTED;
			} else if (logicOp == "or_inverted") {
				return Fox::Vulkan::LogicOperation::OR_INVERTED;
			} else if (logicOp == "nand") {
				return Fox::Vulkan::LogicOperation::NAND;
			} else if (logicOp == "set") {
				return Fox::Vulkan::LogicOperation::SET;
			}

			return Fox::Vulkan::LogicOperation::MAX_LOGIC_OPERATIONS;
		}

		Fox::Vulkan::StencilOperation PipelineConfig::GetStencilOperation(const std::string& stencilOp) {
			if (stencilOp == "keep") {
				return Fox::Vulkan::StencilOperation::KEEP;
			} else if (stencilOp == "zero") {
				return Fox::Vulkan::StencilOperation::ZERO_OP;
			} else if (stencilOp == "replace") {
				return Fox::Vulkan::StencilOperation::REPLACE;
			} else if (stencilOp == "increment_and_clamp") {
				return Fox::Vulkan::StencilOperation::INCREMENT_AND_CLAMP;
			} else if (stencilOp == "decrement_and_clamp") {
				return Fox::Vulkan::StencilOperation::DECREMENT_AND_CLAMP;
			} else if (stencilOp == "invert") {
				return Fox::Vulkan::StencilOperation::INVERT_OP;
			} else if (stencilOp == "increment_and_wrap") {
				return Fox::Vulkan::StencilOperation::INCREMENT_AND_WRAP;
			} else if (stencilOp == "decrement_and_wrap") {
				return Fox::Vulkan::StencilOperation::DECREMENT_AND_WRAP;
			}
			return Fox::Vulkan::StencilOperation::NUM_STENCIL_OPERATIONS;
		}

		Fox::Vulkan::CompareOperation PipelineConfig::GetCompareOperation(const std::string& compareOp) {
			if (compareOp == "never") {
				return Fox::Vulkan::CompareOperation::NEVER;
			} else if (compareOp == "less") {
				return Fox::Vulkan::CompareOperation::LESS;
			} else if (compareOp == "equal") {
				return Fox::Vulkan::CompareOperation::EQUAL;
			} else if (compareOp == "less_or_equal") {
				return Fox::Vulkan::CompareOperation::LESS_OR_EQUAL;
			} else if (compareOp == "greater") {
				return Fox::Vulkan::CompareOperation::GREATER;
			} else if (compareOp == "not_equal") {
				return Fox::Vulkan::CompareOperation::NOT_EQUAL;
			} else if (compareOp == "greater_or_equal") {
				return Fox::Vulkan::CompareOperation::GREATER_OR_EQUAL;
			} else if (compareOp == "always") {
				return Fox::Vulkan::CompareOperation::ALWAYS;
			}
			return Fox::Vulkan::CompareOperation::NUM_COMPARE_OPERATIONS;
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

			// color blending
			Fox::Core::Json::JSONObject& colorBlending = root.Get<Fox::Core::Json::JSONObject>("colorBlending");
			Fox::Core::Json::IntValue& numColorBlendAttachments = colorBlending.Get<Fox::Core::Json::IntValue>("numberOfColorBlendAttachments");
			colorBlendAttachments.resize(numColorBlendAttachments.GetValue());

			Fox::Core::Json::JSONValueArray& colorBlendAttachmentsArray = colorBlending.Get<Fox::Core::Json::JSONValueArray>("colorBlendAttachments");


			for (size_t i = 0u; i < colorBlendAttachments.size(); i++) {

				Fox::Core::Json::JSONObject& colorBlendAttachment = colorBlendAttachmentsArray.Get<Fox::Core::Json::JSONObject>(i);
				Fox::Core::Json::JSONValueArray& colorWriteMaskArray = colorBlendAttachment.Get<Fox::Core::Json::JSONValueArray>("colorWriteMask");

				std::vector<std::string> colorMaskValues;
				for (size_t j = 0u; j < colorWriteMaskArray.Size(); j++) {
					Fox::Core::Json::StringValue& colorMaskString = colorWriteMaskArray.Get<Fox::Core::Json::StringValue>(j);
					colorMaskValues.push_back(colorMaskString.value);
				}

				Fox::Vulkan::ColorWriteMask colorWriteMask = Fox::Vulkan::PipelineConfig::GetColorWriteMask(colorMaskValues);
				Fox::Core::Json::BoolValue& blendEnableBool = colorBlendAttachment.Get<Fox::Core::Json::BoolValue>("blendEnable");
				Fox::Core::Json::StringValue& srcColorBlendFactorString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("srcColorBlendFactor");
				Fox::Vulkan::BlendFactor srcColorBlendFactor = Fox::Vulkan::PipelineConfig::GetBlendFactor(srcColorBlendFactorString.value);
				Fox::Core::Json::StringValue& dstColorBlendFactorString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("dstColorBlendFactor");
				Fox::Vulkan::BlendFactor dstColorBlendFactor = Fox::Vulkan::PipelineConfig::GetBlendFactor(dstColorBlendFactorString.value);
				Fox::Core::Json::StringValue& colorBlendOpString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("colorBlendOp");
				Fox::Vulkan::BlendOperation colorBlendOp = Fox::Vulkan::PipelineConfig::GetBlendOperation(colorBlendOpString.value);
				Fox::Core::Json::StringValue& srcAlphaBlendFactorString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("srcAlphaBlendFactor");
				Fox::Vulkan::BlendFactor srcAlphaBlendFactor = Fox::Vulkan::PipelineConfig::GetBlendFactor(srcAlphaBlendFactorString.value);
				Fox::Core::Json::StringValue& dstAlphaBlendFactorString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("dstAlphaBlendFactor");
				Fox::Vulkan::BlendFactor dstAlphaBlendFactor = Fox::Vulkan::PipelineConfig::GetBlendFactor(dstAlphaBlendFactorString.value);
				Fox::Core::Json::StringValue& alphaBlendOpString = colorBlendAttachment.Get<Fox::Core::Json::StringValue>("alphaBlendOp");
				Fox::Vulkan::BlendOperation alphaBlendOp = Fox::Vulkan::PipelineConfig::GetBlendOperation(alphaBlendOpString.value);

				colorBlendAttachments[i] = {
					colorWriteMask,
					blendEnableBool.GetValue(),
					srcColorBlendFactor,
					dstColorBlendFactor,
					colorBlendOp,
					srcAlphaBlendFactor,
					dstAlphaBlendFactor,
					alphaBlendOp
				};
			}
		
			Fox::Core::Json::BoolValue& logicOpEnableBool = colorBlending.Get<Fox::Core::Json::BoolValue>("logicOpEnable");
			logicOpEnable = logicOpEnableBool.GetValue();
			Fox::Core::Json::StringValue& logicOpString = colorBlending.Get<Fox::Core::Json::StringValue>("logicOp");
			logicOp = Fox::Vulkan::PipelineConfig::GetLogicOperation(logicOpString.value);

			Fox::Core::Json::JSONValueArray& blendConstantsArray = colorBlending.Get<Fox::Core::Json::JSONValueArray>("blendConstants");

			for (size_t i = 0u; i < blendConstantsArray.Size(); i++) {
				Fox::Core::Json::FloatValue blendConstantFloat = blendConstantsArray.Get<Fox::Core::Json::FloatValue>(i);
				blendConstants[i] = blendConstantFloat.GetValue();
			}

			Fox::Core::Json::JSONObject& depthStencil = root.Get<Fox::Core::Json::JSONObject>("depthStencil");
			Fox::Core::Json::BoolValue& depthTestEnableBool = depthStencil.Get<Fox::Core::Json::BoolValue>("depthTestEnable");
			depthTestEnable = depthTestEnableBool.GetValue();
			Fox::Core::Json::BoolValue& depthWriteEnableBool = depthStencil.Get<Fox::Core::Json::BoolValue>("depthWriteEnable");
			depthWriteEnable = depthWriteEnableBool.GetValue();
			Fox::Core::Json::StringValue& depthCompareOpString = depthStencil.Get<Fox::Core::Json::StringValue>("depthCompareOp");
			depthCompareOp = Fox::Vulkan::PipelineConfig::GetCompareOperation(depthCompareOpString.value);
			Fox::Core::Json::BoolValue& depthBoundsTestEnableBool = depthStencil.Get<Fox::Core::Json::BoolValue>("depthBoundsTestEnable");
			depthBoundsTestEnable = depthBoundsTestEnableBool.GetValue();
			Fox::Core::Json::FloatValue& minDepthBoundsFloat = depthStencil.Get<Fox::Core::Json::FloatValue>("minDepthBounds");
			minDepthBounds = minDepthBoundsFloat.GetValue();
			Fox::Core::Json::FloatValue& maxDepthBoundsFloat = depthStencil.Get<Fox::Core::Json::FloatValue>("maxDepthBounds");
			maxDepthBounds = maxDepthBoundsFloat.GetValue();
			Fox::Core::Json::BoolValue& stencilTestEnableBool = depthStencil.Get<Fox::Core::Json::BoolValue>("stencilTestEnable");
			stencilTestEnable = stencilTestEnableBool.GetValue();

			Fox::Core::Json::JSONObject& frontStateObject = depthStencil.Get<Fox::Core::Json::JSONObject>("frontState");
			Fox::Core::Json::StringValue& frontFaceFailOpString = frontStateObject.Get<Fox::Core::Json::StringValue>("failOp");
			Fox::Core::Json::StringValue& frontFacePassOpString = frontStateObject.Get<Fox::Core::Json::StringValue>("passOp");
			Fox::Core::Json::StringValue& frontFaceDepthFailOpString = frontStateObject.Get<Fox::Core::Json::StringValue>("depthFailOp");
			Fox::Core::Json::StringValue& frontFaceCompareOpString = frontStateObject.Get<Fox::Core::Json::StringValue>("compareOp");

			Fox::Core::Json::IntValue& frontFaceCompareMaskInt = frontStateObject.Get<Fox::Core::Json::IntValue>("compareMask");
			Fox::Core::Json::IntValue& frontFaceWriteMaskInt = frontStateObject.Get<Fox::Core::Json::IntValue>("writeMask");
			Fox::Core::Json::IntValue& frontFaceReferenceInt = frontStateObject.Get<Fox::Core::Json::IntValue>("reference");


			frontState = {
				Fox::Vulkan::PipelineConfig::GetStencilOperation(frontFaceFailOpString.value),
				Fox::Vulkan::PipelineConfig::GetStencilOperation(frontFacePassOpString.value),
				Fox::Vulkan::PipelineConfig::GetStencilOperation(frontFaceDepthFailOpString.value),
				Fox::Vulkan::PipelineConfig::GetCompareOperation(frontFaceCompareOpString.value),
				static_cast<uint32_t>(frontFaceCompareMaskInt.GetValue()),
				static_cast<uint32_t>(frontFaceWriteMaskInt.GetValue()),
				static_cast<uint32_t>(frontFaceReferenceInt.GetValue())
			};

			Fox::Core::Json::JSONObject& backStateObject = depthStencil.Get<Fox::Core::Json::JSONObject>("backState");
			Fox::Core::Json::StringValue& backFaceFailOpString = backStateObject.Get<Fox::Core::Json::StringValue>("failOp");
			Fox::Core::Json::StringValue& backFacePassOpString = backStateObject.Get<Fox::Core::Json::StringValue>("passOp");
			Fox::Core::Json::StringValue& backFaceDepthFailOpString = backStateObject.Get<Fox::Core::Json::StringValue>("depthFailOp");
			Fox::Core::Json::StringValue& backFaceCompareOpString = backStateObject.Get<Fox::Core::Json::StringValue>("compareOp");

			Fox::Core::Json::IntValue& backFaceCompareMaskInt = backStateObject.Get<Fox::Core::Json::IntValue>("compareMask");
			Fox::Core::Json::IntValue& backFaceWriteMaskInt = backStateObject.Get<Fox::Core::Json::IntValue>("writeMask");
			Fox::Core::Json::IntValue& backFaceReferenceInt = backStateObject.Get<Fox::Core::Json::IntValue>("reference");


			backState = {
				Fox::Vulkan::PipelineConfig::GetStencilOperation(backFaceFailOpString.value),
				Fox::Vulkan::PipelineConfig::GetStencilOperation(backFacePassOpString.value),
				Fox::Vulkan::PipelineConfig::GetStencilOperation(backFaceDepthFailOpString.value),
				Fox::Vulkan::PipelineConfig::GetCompareOperation(backFaceCompareOpString.value),
				static_cast<uint32_t>(backFaceCompareMaskInt.GetValue()),
				static_cast<uint32_t>(backFaceWriteMaskInt.GetValue()),
				static_cast<uint32_t>(backFaceReferenceInt.GetValue())
			};

		}
	}
}


