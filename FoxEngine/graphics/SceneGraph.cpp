#include "pch.h"

namespace Fox {

	namespace Vulkan {
	
		void SceneGraph::Destroy() {
			ForEach([=](std::shared_ptr<Fox::Vulkan::SceneNode> node) {
				node->~SceneNode();
			});
		}


		void SceneGraph::AddChild(std::string name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
		}

		std::shared_ptr<Fox::Vulkan::SceneNode>& SceneGraph::AddChild(std::string name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, std::string modelPath) {
			return root->AddChild(name, position, rotation, scale, modelPath);
		}

		void SceneGraph::Update() {
			UpdateInternal(root);
		}

		void SceneGraph::UpdateInternal(std::shared_ptr<Fox::Vulkan::SceneNode>& parent) {
			for (auto& child : parent->children) {
				if (child->needsUpdate) {
					child->UpdateLocalTransform();
					child->UpdateWorldTransform();
				}
				UpdateInternal(child);
			}
		}

		void SceneGraph::ForEach(std::function<void(std::shared_ptr<Fox::Vulkan::SceneNode>)> function) {
			TraverseGraph(root, function);
		}

		void SceneGraph::TraverseGraph(std::shared_ptr<Fox::Vulkan::SceneNode>& node, std::function<void(std::shared_ptr<Fox::Vulkan::SceneNode>)> function) {
			for (auto& child : node->children) {
				function(child);
				TraverseGraph(child, function);
			}
		}

		Fox::Vulkan::SceneNode* SceneGraph::Find(std::string name) {
			Fox::Vulkan::SceneNode* value = nullptr;
			ForEach([&name, &value] (std::shared_ptr <Fox::Vulkan::SceneNode> node) {
				if (node->name == name) {
					value = node.get();
					return;
				}
			});

			return value;
		}


	}

}