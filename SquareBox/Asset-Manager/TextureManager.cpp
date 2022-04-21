#include "TextureManager.h"

namespace SquareBox {
	namespace AssetManager {
		TextureCache TextureManager::m_texture_cache;//since it a static variable we had to re declare it

		GLTexture TextureManager::getTextureByFilePath(std::string texture_path_)
		{//this is a wrapper function because all it does is call another function
			return m_texture_cache.getTexture(texture_path_);
		}

		GLTexture TextureManager::getTextureByName(std::string texture_name_, float dpi_)
		{
			std::string image_category;
			if (dpi_ >= 640) {
				image_category = "xxxhdpi/";
			}
			else if (dpi_ >= 480) {
				image_category = "xxhdpi/";
			}
			else if (dpi_ >= 320) {
				image_category = "xhdpi/";
			}
			else if (dpi_ >= 240) {
				image_category = "hdpi/";
			}
			else if (dpi_ >= 160) {
				image_category = "mdpi/";
			}
			else if (dpi_ >= 120) {
				image_category = "ldpi/";
			}
			else {
				image_category = "nodpi/";
			}
			std::string texure_path = "Assets/Textures/" + image_category + texture_name_;
			return m_texture_cache.getTexture(texure_path);
		}
		GLTexture TextureManager::getTextureById(int id_)
		{
			return m_texture_cache.getTexture(id_);
		}
		void TextureManager::setTextureTilingById(int id_, glm::vec2 tiling_)
		{
			m_texture_cache.setTextureTilingById(id_,tiling_);
		}
	}
}