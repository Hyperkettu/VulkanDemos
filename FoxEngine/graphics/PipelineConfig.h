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

		struct ColorWriteMask {
			bool red;
			bool green;
			bool blue;
			bool alpha;
		};

		enum BlendFactor {
			ZERO = 0,
		    ONE = 1,
			SRC_COLOR = 2,
			ONE_MINUS_SRC_COLOR = 3,
			DST_COLOR = 4,
			ONE_MINUS_DST_COLOR = 5,
			SRC_ALPHA = 6,
			ONE_MINUS_SRC_ALPHA = 7,
			DST_ALPHA = 8,
			ONE_MINUS_DST_ALPHA = 9,
			CONSTANT_COLOR = 10,
			ONE_MINUS_CONSTANT_COLOR = 11,
			CONSTANT_ALPHA = 12,
			ONE_MINUS_CONSTANT_ALPHA = 13,
			SRC_ALPHA_SATURATE = 14,
			SRC1_COLOR = 15,
			ONE_MINUS_SRC1_COLOR = 16,
			SRC1_ALPHA = 17,
			ONE_MINUS_SRC1_ALPHA = 18,
			NUM_BLEND_FACTORS
		};

		enum BlendOperation {
			ADD = 0,
			SUBTRACT = 1,
			REVERSE_SUBTRACT = 2,
			MIN = 3,
			MAX = 4,
			ZERO_EXT = 1000148000,
			SRC_EXT = 1000148001,
			DST_EXT = 1000148002,
			SRC_OVER_EXT = 1000148003,
			DST_OVER_EXT = 1000148004,
			SRC_IN_EXT = 1000148005,
			DST_IN_EXT = 1000148006,
			SRC_OUT_EXT = 1000148007,
			DST_OUT_EXT = 1000148008,
			SRC_ATOP_EXT = 1000148009,
			DST_ATOP_EXT = 1000148010,
			XOR_EXT = 1000148011,
			MULTIPLY_EXT = 1000148012,
			SCREEN_EXT = 1000148013,
			OVERLAY_EXT = 1000148014,
			DARKEN_EXT = 1000148015,
			LIGHTEN_EXT = 1000148016,
			COLORDODGE_EXT = 1000148017,
			COLORBURN_EXT = 1000148018,
			HARDLIGHT_EXT = 1000148019,
			SOFTLIGHT_EXT = 1000148020,
			DIFFERENCE_EXT = 1000148021,
			EXCLUSION_EXT = 1000148022,
			INVERT_EXT = 1000148023,
			INVERT_RGB_EXT = 1000148024,
			LINEARDODGE_EXT = 1000148025,
			LINEARBURN_EXT = 1000148026,
			VIVIDLIGHT_EXT = 1000148027,
			LINEARLIGHT_EXT = 1000148028,
			PINLIGHT_EXT = 1000148029,
			HARDMIX_EXT = 1000148030,
			HSL_HUE_EXT = 1000148031,
			HSL_SATURATION_EXT = 1000148032,
			HSL_COLOR_EXT = 1000148033,
			HSL_LUMINOSITY_EXT = 1000148034,
			PLUS_EXT = 1000148035,
			PLUS_CLAMPED_EXT = 1000148036,
			PLUS_CLAMPED_ALPHA_EXT = 1000148037,
			PLUS_DARKER_EXT = 1000148038,
			MINUS_EXT = 1000148039,
			MINUS_CLAMPED_EXT = 1000148040,
			CONTRAST_EXT = 1000148041,
			INVERT_OVG_EXT = 1000148042,
			RED_EXT = 1000148043,
			GREEN_EXT = 1000148044,
			BLUE_EXT = 1000148045,
			NUM_BLEND_OPERATIONS = 51
		};

		struct ColorBlendAttachment {
			Fox::Vulkan::ColorWriteMask colorWriteMask;
			bool blendEnable;
			Fox::Vulkan::BlendFactor srcColorBlendFactor;
			Fox::Vulkan::BlendFactor dstColorBlendFactor;
			Fox::Vulkan::BlendOperation colorBlendOp;
			Fox::Vulkan::BlendFactor srcAlphaBlendFactor;
			Fox::Vulkan::BlendFactor dstAlphaBlendFactor;
			Fox::Vulkan::BlendOperation alphaBlendOp;
		};

		enum LogicOperation {
			CLEAR = 0,
			AND = 1,
			AND_REVERSE = 2,
			COPY = 3,
			AND_INVERTED = 4,
			NO_OP = 5,
			XOR = 6,
			OR = 7,
			NOR = 8,
			EQUIVALENT = 9,
			INVERT = 10,
			OR_REVERSE = 11,
			COPY_INVERTED = 12,
			OR_INVERTED = 13,
			NAND = 14,
			SET = 15,
			MAX_LOGIC_OPERATIONS = 16
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
			static Fox::Vulkan::ColorWriteMask GetColorWriteMask(std::vector<std::string>& mask);
			static Fox::Vulkan::BlendFactor GetBlendFactor(const std::string& blendFactor);
			static Fox::Vulkan::BlendOperation GetBlendOperation(const std::string& blendOp);
			static Fox::Vulkan::LogicOperation GetLogicOperation(const std::string& logicOp);

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

			// color blending
			std::vector<Fox::Vulkan::ColorBlendAttachment> colorBlendAttachments;
			bool logicOpEnable;
			Fox::Vulkan::LogicOperation logicOp;
			float blendConstants[4];
		};

	}
}