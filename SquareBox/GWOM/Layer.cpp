#include "Layer.h"

std::pair<int, glm::vec4> SquareBox::GWOM::Layer::getTextureIdAndUvReactFromKey(int key_)
{
	glm::vec4 texture_uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	int texture_id = 0;
	if (key_ != -1) {

		std::map<int, SquareBox::GWOM::SubTexture>::iterator sub_texture_it = sub_textures_table.find(key_);
		if (sub_texture_it == sub_textures_table.end()) {
			/*
				can happen when we have placed a tile in a world with no loaded textures this results into
				a black tile being drawn.
			*/
		}
		else {
			const auto & target_sub_texture = (*sub_texture_it).second;

			if (target_sub_texture.parent_type == SquareBox::TextureEnum::SINGLE) {
				texture_id = single_textures[target_sub_texture.parent_texture_index].texture.id;
				//uvReact remains unchanged
			}
			else if (target_sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET) {
				texture_id = tiled_textures[target_sub_texture.parent_texture_index].texture.id;
				texture_uvRect = tiled_textures[target_sub_texture.parent_texture_index].texture.getUVReactAtIndex(target_sub_texture.tiling_index, glm::ivec2(0));
			}

		}
	}
	return std::pair<int, glm::vec4>(texture_id,texture_uvRect);
}
