#pragma once

#include "graphics/Renderer.h"

namespace Fox {
	
	namespace Vulkan {

		class Renderer;

		template<class V>
		class MeshBase {
		public: 
			MeshBase() = default;
			virtual ~MeshBase() {};

			Fox::Vulkan::Buffer<V>* GetVertices() const {
				return vertices;
			}

		protected: 
			Fox::Vulkan::Buffer<V>* vertices;
		};

		template<class V, class I>
		class IndexedMesh : public Fox::Vulkan::MeshBase<V> {

		public: 
			IndexedMesh() = default;
			
			IndexedMesh(const std::vector<V>& vertices, const std::vector<I>& indices) {
				Fox::Vulkan::Renderer* renderer = Fox::Vulkan::Renderer::GetRenderer();

				VkDeviceSize vertexBufferSize = sizeof(V) * vertices.size();
				Fox::Vulkan::Buffer<V> vertexStagingBuffer;

				vertexStagingBuffer.Create(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
				vertexStagingBuffer.SetContents(vertices);

				this->vertices = new Fox::Vulkan::Buffer<V>();
				this->vertices->Create(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				this->vertices->SetContents(vertices, false);
				renderer->CopyBuffer(vertexStagingBuffer.GetBuffer(), this->vertices->GetBuffer(), vertexBufferSize);

				VkDeviceSize indexBufferSize = sizeof(I) * indices.size();
				Fox::Vulkan::Buffer<I> indexStagingBuffer;

				indexStagingBuffer.Create(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
				indexStagingBuffer.SetContents(indices);

				this->indices = new Fox::Vulkan::Buffer<I>();
				this->indices->Create(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				this->indices->SetContents(indices, false);
				renderer->CopyBuffer(indexStagingBuffer.GetBuffer(), this->indices->GetBuffer(), indexBufferSize);

			}

			virtual ~IndexedMesh() {
				if (this->vertices) {
					delete this->vertices;
					this->vertices = nullptr;
				}
				if (this->indices) {
					delete this->indices;
					this->indices = nullptr;
				}
			}

			Fox::Vulkan::Buffer<I>* GetIndices() {
				return indices;
			}

			size_t GetIndexCount() const {
				return indices->GetElementCount();
			}

		private:
			Fox::Vulkan::Buffer<I>* indices;
		};
	

		using Mesh = Fox::Vulkan::IndexedMesh<Fox::Vulkan::Vertex, uint32_t>;
	}
}