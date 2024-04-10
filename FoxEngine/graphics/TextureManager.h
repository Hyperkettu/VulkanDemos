#pragma once

namespace Fox {

	namespace Vulkan {

		class TextureManager {

		public:

			TextureManager() = default;
			~TextureManager();

			VkImageView GetImageViewForTexture() {
				return texture->GetImageView();
			}
			void CreateTextures(uint32_t mipLevels);

		private:

			const std::string TEXTURE_PATH = "textures/viking.png";


			void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

			std::shared_ptr<Fox::Vulkan::Texture> texture;


		};
	}
}