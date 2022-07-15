#include "TextureCache.h"
#include "ImageLoader.h"

namespace SquareBox {
	namespace AssetManager {
		TextureCache::TextureCache()
		{
		}

		TextureCache::~TextureCache()
		{
		}

		GLTexture TextureCache::getTexture(std::string texture_path_)
		{
			std::map<std::string, GLTexture>::iterator mit = m_file_path_texture_map.find(texture_path_);
			// check and see if we found it

			if (mit == m_file_path_texture_map.end()) {
				//since we didn't find it lets load it in
				GLTexture new_texture = ImageLoader::getTextureFromImageFile(texture_path_);

				//insert the new texture into our map
				m_file_path_texture_map.insert(make_pair(texture_path_, new_texture));
				m_id_texture_map[new_texture.id] = new_texture;// keep the ids map upto date
				SBX_CORE_INFO("Live Loaded  Texture  {} ", texture_path_);
				return new_texture;
			}
			else {
				SBX_CORE_INFO("Cache Loaded  Texture  {} ", texture_path_);
			}
			return mit->second;//the element that the pointer is pointing to , which is the Texture
		}
		GLTexture TextureCache::getTexture(int texture_id_)
		{
			std::map<int, GLTexture>::iterator mit = m_id_texture_map.find(texture_id_);

			if (mit == m_id_texture_map.end()) {
				//We didn't find the texture. 
				SBX_CORE_ERROR("Invalid Texture Id {} , no corresponding texture found", texture_id_);
				return GLTexture();
			}
			return mit->second;
		}
		void TextureCache::setTextureTilingById(int texture_id_, glm::ivec2 tiling_)
		{
			std::map<int, GLTexture>::iterator mit = m_id_texture_map.find(texture_id_);
			if (mit != m_id_texture_map.end()) {
				// it exists so lets edit its tiling
				m_id_texture_map[texture_id_].tiling = tiling_;

				//Edit the file path copy as well
				std::map<std::string, GLTexture>::iterator mit_2 = m_file_path_texture_map.find(mit->second.asset_file_path);

				if (mit_2 != m_file_path_texture_map.end()) {
					m_file_path_texture_map[mit->second.asset_file_path].tiling = tiling_;
				}

			}
			else {
				SBX_CORE_ERROR("Invalid Texture Id {} , no corresponding texture found",texture_id_);
			}
		}
		void TextureCache::setTextureDisplayNameById(int texture_id_, std::string display_name_)
		{
			std::map<int, GLTexture>::iterator mit = m_id_texture_map.find(texture_id_);
			if (mit != m_id_texture_map.end()) {
				// it exists so lets edit its tiling
				m_id_texture_map[texture_id_].display_name = display_name_;

				//Edit the file path copy as well
				std::map<std::string, GLTexture>::iterator mit_2 = m_file_path_texture_map.find(mit->second.asset_file_path);

				if (mit_2 != m_file_path_texture_map.end()) {
					m_file_path_texture_map[mit->second.asset_file_path].display_name = display_name_;
				}

			}
			else {
				SBX_CORE_ERROR("Invalid Texture Id {} , no corresponding texture found", texture_id_);
			}
		}
	}
}