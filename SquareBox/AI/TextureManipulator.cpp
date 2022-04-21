#include "TextureManipulator.h"

namespace SquareBox {
	namespace AI {
		TextureManipulator::TextureManipulator(SquareBox::GWOM::ClusterObject & cluster_object_, int start_tile_, int num_tiles_, float duration_)
		{
			manipulation_object_name = cluster_object_.name;
			manipulation_duration = duration_;
			manipulation_complete = false;
			m_num_tiles = num_tiles_;
			m_start_tile = start_tile_;

			m_gradual_tile_gain = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(num_tiles_), duration_);
		}
		bool TextureManipulator::update(float delta_time_, SquareBox::GWOM::ClusterObject & cluster_object_)
		{
			m_accumulated_tile_gain += m_gradual_tile_gain * delta_time_;
			bool move_to_next_tile = false;
			int current_tile = cluster_object_.texture_info.tile_sheet_index;
			int new_tile = current_tile;

			if (m_accumulated_tile_gain > 1) {
				m_accumulated_tile_gain -= 1;
				new_tile = current_tile + 1; // we shall move through tiles in a step step manner

				int texture_index_cap = m_start_tile + m_num_tiles;
				if (new_tile >= texture_index_cap) {
					new_tile = texture_index_cap;
					manipulation_complete = true;
				}
				const SquareBox::AssetManager::GLTexture & object_texture = SquareBox::AssetManager::TextureManager::getTextureById(cluster_object_.texture_info.texture_id);
				if (static_cast<int>(object_texture.id) <= 0) {
					SBX_CORE_ERROR("{} texture id of {} did not return a valid texture", cluster_object_.name, cluster_object_.texture_info.texture_id);
				}
				SquareBox::AssetManager::TileSheet tilesheet;
				tilesheet.readGrid(object_texture);
				//now actually sending our updated texture tiling
				cluster_object_.texture_info.tile_sheet_index = new_tile;
				cluster_object_.texture_info.uv_rect = tilesheet.getUVCords(new_tile);
			}




			return manipulation_complete;
		}
	};
};