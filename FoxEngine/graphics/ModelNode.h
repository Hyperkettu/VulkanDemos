#pragma once

namespace Fox {

	namespace Vulkan {

		class ModelNode: public Fox::Vulkan::SceneNode {
		public:
			ModelNode() = default;
			ModelNode(std::string name, std::string modelFile): Fox::Vulkan::SceneNode(name) {
				model->Load(modelFile);
			}
			ModelNode(ModelNode& node) {}
			virtual ~ModelNode() {
				model = nullptr;
			}

			void LoadModel(std::string modelPath);
			Fox::Vulkan::Model* GetModel() {
				return model.get();
			}

		private: 
			std::shared_ptr<Fox::Vulkan::Model> model;
		};
	}
}