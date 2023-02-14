#pragma once
#include "TextureCache.h"
#include <string>
namespace SquareBox {
	namespace AssetManager {
		class TextureManager
		{
		public:

			static GLTexture createTextureFromFileBuffer(std::pair<float*, int> file_buffer_info_,int width_,int height_);

			/// returns texture basing on its Id
			static GLTexture getTextureByFilePath(std::string texture_path_);

			/// the texture here is got from the sutiable folder in the assets folder 
			///	This ensures the best resolution on any screen if different textures resolutions
			/// for different screens were provided 
			static GLTexture getTextureByName(std::string texture_name_, float dpi_);
			/// the texture is retrieved from cached textures based on its id
			static GLTexture getTextureById(int id_);
			/// Applies the tiling to the cached version of the Texture so that it can be maintained 
			/// when being retrieved from the asset manager 
			static void setTextureTilingById(int id_, glm::ivec2 tiling_);
			static void setTextureDisplayNameById(int id_, std::string display_name_);

		
		private:
			static TextureCache m_texture_cache;
		};
	}
}