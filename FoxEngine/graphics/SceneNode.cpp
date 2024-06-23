#include "pch.h"

namespace Fox {

	namespace Vulkan {
	
		template<class NodeType, class ParentType>
		std::shared_ptr<NodeType>& SceneNode::AddChild(std::string name, glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
			std::shared_ptr<NodeType> node = std::make_shared<NodeType>();
			
			node->parent = shared_from_this();
			node->name = name;
			node->position = position;
			node->rotation = rotation;
			node->scale = scale;
			node->needsUpdate = true;

			children.push_back(node);
			needsUpdate = true;
			
			return node;
		}

		std::shared_ptr<Fox::Vulkan::SceneNode>& SceneNode::AddChild(std::string name, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::string modelPath) {
			std::shared_ptr<Fox::Vulkan::ModelNode>& node = AddChild<Fox::Vulkan::ModelNode, Fox::Vulkan::SceneNode>(name, position, rotation, scale);
			node->LoadModel(modelPath);
			return (std::shared_ptr<Fox::Vulkan::SceneNode>&) node;
		}

		void SceneNode::UpdateLocalTransform() {
			localTransform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
		}

		void SceneNode::UpdateWorldTransform() {
			worldTransform = parent->worldTransform * localTransform;
		}

		void SceneNode::SetRotation(float pitch, float yaw, float roll) {
			glm::vec3 eulerAngles(glm::radians(pitch), glm::radians(yaw), glm::radians(roll));
			rotation = glm::qua(eulerAngles);
			needsUpdate = true;
		}

		void SceneNode::SetPosition(float x, float y, float z) {
			position.x = x;
			position.y = y;
			position.z = z;
			needsUpdate = true;
		}

		void SceneNode::SetScale(float scaleX, float scaleY, float scaleZ) {
			scale.x = scaleX;
			scale.y = scaleY;
			scale.z = scaleZ;
			needsUpdate = true;
		}

	}
}