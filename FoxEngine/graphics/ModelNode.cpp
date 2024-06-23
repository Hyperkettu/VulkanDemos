#include "pch.h"

namespace Fox {

	namespace Vulkan {

		void ModelNode::LoadModel(std::string modelPath) {
			model = std::make_shared<Fox::Vulkan::Model>();
			model->Load(modelPath);
		}
	
	}
}