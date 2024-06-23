#pragma once

namespace Fox {

	namespace Vulkan {

		class SceneNode: public std::enable_shared_from_this<SceneNode> {
		public:

			SceneNode() = default;

			SceneNode(std::string name) : 
				name(name), 
				position(0.0f, 0.0f, 0.0f), 
				rotation(1.0f, glm::vec3(0.0f, 0.0f, 0.0f)), 
				scale(1.0f, 1.0f, 1.0f),
				localTransform(glm::mat4(1.0f)),
				worldTransform(glm::mat4(1.0f)) {}

			SceneNode(SceneNode& sceneNode) {}

			virtual ~SceneNode() {}

			template<class NodeType, class ParentType>
			std::shared_ptr<NodeType>& AddChild(std::string name, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
			std::shared_ptr<Fox::Vulkan::SceneNode>& AddChild(std::string name, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::string modelPath);
			void UpdateLocalTransform();
			void UpdateWorldTransform();

			void SetRotation(float pitch, float yaw, float roll);
			void SetPosition(float x, float y, float z);
			void SetScale(float scaleX, float scaleY, float scaleZ);

			std::string name;
			std::shared_ptr<Fox::Vulkan::SceneNode> parent;
			std::vector<std::shared_ptr<Fox::Vulkan::SceneNode>> children;

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			glm::mat4 localTransform;
			glm::mat4 worldTransform;
		
			bool needsUpdate = false;
		};
	}
}