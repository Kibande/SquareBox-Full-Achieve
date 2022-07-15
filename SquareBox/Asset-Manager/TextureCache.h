#pragma once
#include<map>
#include "GLTexture.h"
#include <iostream>
namespace SquareBox {
	namespace AssetManager {
		class TextureCache
		{
		public:
			TextureCache();
			~TextureCache();
			GLTexture getTexture(std::string texture_file_path_);
			GLTexture getTexture(int texture_id_);
			void setTextureTilingById(int texture_id_, glm::ivec2 tiling_);
			void setTextureDisplayNameById(int texture_id_, std::string display_name_);
		private:
			std::map<std::string, GLTexture> m_file_path_texture_map;
			std::map<int, GLTexture> m_id_texture_map;
		};
	}
}