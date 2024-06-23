#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <optional>
#include <set>
#include <cstdint> 
#include <limits> 
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>
#include <random>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_Vulkan.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include<glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Fox {

	namespace Vulkan {

		struct PerFrameConstantBuffer {
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
		};

		struct PerObjectConstantBuffer {
			alignas(16) glm::mat4 model;
		};
	}
}

#include "core/JSON.h"

#include "graphics/Vertex.h"
#include "graphics/RendererConfig.h"
#include "graphics/Buffer.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/Model.h"
#include "graphics/SceneNode.h"
#include "graphics/ModelNode.h"
#include "graphics/SceneGraph.h"
#include "graphics/Swapchain.h"
#include "core/FileSystem.h"
#include "graphics/Synchronization.h"
#include "graphics/ConstantBuffers.h"
#include "graphics/TextureManager.h"
#include "graphics/SamplerManager.h"
#include "graphics/DescriptorSetManager.h"
#include "graphics/RenderPassManager.h"
#include "graphics/PipelineConfig.h"
#include "graphics/GraphicsPipelineState.h"
#include "graphics/Renderer.h"

