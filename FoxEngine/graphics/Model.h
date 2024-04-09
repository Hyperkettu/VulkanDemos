#pragma once

namespace Fox {

	namespace Vulkan {
	
		class Model {
		public: 
			Model() {}
			~Model();

			VkBuffer GetVertexBuffer() {
				return mesh->GetVertices()->GetBuffer();
			}

			VkBuffer GetIndexBuffer() {
				return mesh->GetIndices()->GetBuffer();
			}

			size_t GetIndexCount() {
				return mesh->GetIndexCount();
			}

			void load(const std::string& path);
		private:

			std::shared_ptr<Mesh> mesh;
		};
	}
}