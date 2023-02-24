#pragma once
#include <SquareBox-Core/Essentials.h>
#include<glm/glm.hpp>
namespace SquareBox {
	namespace GWOM {

		class TextureInfo {
		public:
			TextureEnum texture_type = TextureEnum::SINGLE;//type of texture a single texture of a tilesheet
			int texture_index = 0;//the textures position in the table of textures 
			int tile_sheet_index = 0;//For if we have a tile sheet
			int texture_id=0;/* this is for easy accessing and it can be used against the asset manager to query
						   for more information about the texture like the file path name , original dimensions
						   generally all the information that can be found in the SquareBox::AssetManager::GLTexture object
						  */
						   //vec4
			glm::vec4 uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);//textures uvLayer out
			glm::ivec4 color = glm::ivec4(255, 255, 255, 255);
			int opacity = 100;
			glm::ivec2 inversions = glm::ivec2(0); // determine whether the texture should be inverted in either the x or y axis
		};

	};
};
