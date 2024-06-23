#pragma once

namespace Fox {

	namespace Vulkan {

		class SceneGraph {
		public:
			SceneGraph() {
				root = std::make_shared<Fox::Vulkan::SceneNode>("root");
			}
			~SceneGraph() {}

			void Destroy();

			void AddChild(std::string name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
			std::shared_ptr<Fox::Vulkan::SceneNode>& AddChild(std::string name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, std::string modelPath);


			void Update();
			void UpdateInternal(std::shared_ptr<Fox::Vulkan::SceneNode>& parent);

			void ForEach(std::function<void(std::shared_ptr<Fox::Vulkan::SceneNode>)> function);

			Fox::Vulkan::SceneNode* Find(std::string name);

		private:

			void TraverseGraph(std::shared_ptr<Fox::Vulkan::SceneNode>& node, std::function<void(std::shared_ptr<Fox::Vulkan::SceneNode>)> function);

			std::shared_ptr<Fox::Vulkan::SceneNode> root;
		};
	}
}