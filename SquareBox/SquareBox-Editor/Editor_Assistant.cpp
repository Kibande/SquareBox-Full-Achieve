#include "Editor_Assistant.h"
void SquareBoxEditor::Editor_Assistant::currentTileDuplicator(SquareBox::GWOM::Layer& active_layer_ref_, std::vector<std::pair<int, int>>& selected_tiles_vec_, SquareBox::IMainGame* m_game, SquareBox::Utilities& m_utilities)
{
	// we are going to loop through , duplicating tile by tile
	std::vector<std::pair<int, int>> duplicates;
	if (
		m_game->getInputDevice()->isInputIdBeingReceived(cluster_object_selection_super_duplicate_input_key_1)
		&&
		(
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)
			||

			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_east_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_east_duplication_input_key)
			)
		) {
		//duplicate the selected cells but with the apropriate texture
		for (unsigned int i = 0; i < selected_tiles_vec_.size(); i++)
		{
			auto* focus_tile = active_layer_ref_.tile_system.getTileByIndex(selected_tiles_vec_[i].second);

			// first check for diagonals
			if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_west_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index - 1 + retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_east_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first + 1;
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index + 1 + retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_west_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index - 1 - retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_east_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first + 1;
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index + 1 - retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}

			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)) {
				//duplicate into the tile on the west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, focus_tile->coordinates.second);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index - 1;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)) {
				//duplicate into the tile on the east
				int new_tile_row = focus_tile->coordinates.first + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, focus_tile->coordinates.second);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index + 1;
							// check that we are still in the tiling boundaries
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)) {
				//duplicate into the tile on the north
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(focus_tile->coordinates.first, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index + retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)) {
				//duplicate into the tile on the south
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(focus_tile->coordinates.first, new_tile_col);

					if (focus_tile->key != -1) {

						auto  focus_tile_sub_raw_data = active_layer_ref_.sub_textures_table.find(focus_tile->key);
						if (focus_tile_sub_raw_data != active_layer_ref_.sub_textures_table.end()) {

							const auto& focus_tile_sub_texture = focus_tile_sub_raw_data->second;
							int focus_tiling_index = focus_tile_sub_texture.tiling_index;

							auto results = active_layer_ref_.getTextureIdAndUvReactFromKey(focus_tile_sub_texture.sub_texture_key);

							auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(results.first);

							int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;

							int desired_target_tile_tiling_index = focus_tiling_index - retrieved_texture.tiling.x;
							// check that we are still in the tiling boundaries
							if (desired_target_tile_tiling_index < 0) {
								return;
							}
							else if (desired_target_tile_tiling_index >= retrieved_texture_max_index) {
								return;
							}

							for (unsigned int j = 0; j < active_layer_ref_.sub_textures_table.size(); j++) {
								const auto& sub_texture = active_layer_ref_.sub_textures_table[j];
								if (
									(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
									&&

									(sub_texture.tiling_index == desired_target_tile_tiling_index)
									) {

									target_tile->key = sub_texture.sub_texture_key;
									break;
								}
							}
						}
						else {
							SBX_CORE_CRITICAL("Sub texture not in sub textures table");
							__debugbreak();

						}

					}

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
		}
		if (duplicates.size() > 0) {
			selected_tiles_vec_.clear();
			selected_tiles_vec_ = duplicates;
		}

	}
	else if (
		m_game->getInputDevice()->isInputIdBeingReceived(cluster_object_selection_duplication_input_key_1)
		&&
		(
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)
			||

			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_east_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_west_duplication_input_key)
			||
			m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_east_duplication_input_key)
			)
		) {

		for (unsigned int i = 0; i < selected_tiles_vec_.size(); i++)
		{
			auto* focus_tile = active_layer_ref_.tile_system.getTileByIndex(selected_tiles_vec_[i].second);

			// first check for diagonals
			if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_west_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_east_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first + 1;
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_west_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_east_duplication_input_key)) {

				//duplicate into the tile on the north west
				int new_tile_row = focus_tile->coordinates.first + 1;
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index && new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}

			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)) {
				//duplicate into the tile on the west
				int new_tile_row = focus_tile->coordinates.first - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, focus_tile->coordinates.second);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)) {
				//duplicate into the tile on the east
				int new_tile_row = focus_tile->coordinates.first + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_row >= 0 && new_tile_row <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(new_tile_row, focus_tile->coordinates.second);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)) {
				//duplicate into the tile on the north
				int new_tile_col = focus_tile->coordinates.second + 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(focus_tile->coordinates.first, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
			else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)) {
				//duplicate into the tile on the south
				int new_tile_col = focus_tile->coordinates.second - 1;
				int max_tile_index = (active_layer_ref_.tile_system.getNumXCells() * active_layer_ref_.tile_system.getNumYCells()) - 1;
				if (new_tile_col >= 0 && new_tile_col <= max_tile_index) {
					//populate the tile
					auto target_tile = active_layer_ref_.tile_system.getTile(focus_tile->coordinates.first, new_tile_col);

					target_tile->key = focus_tile->key;

					m_utilities.addPairToVector(active_layer_ref_.tile_system.active_tiles, std::pair<int, int>(target_tile->coordinates.first, target_tile->coordinates.second));
					duplicates.push_back(std::pair<int, int>(active_layer_ref_.index, target_tile->index));
				}
			}
		}
		if (duplicates.size() > 0) {
			selected_tiles_vec_.clear();
			selected_tiles_vec_ = duplicates;
		}

	}

}


/*
	the code in this class is designed to be able to practically be liftable from here and thrown into the LevelEditor
	in the places where the function is called
*/

// we need to modify this so that we can duplicate a vector of cluster objects and not only one
std::pair<int, int> SquareBoxEditor::Editor_Assistant::currentClusterObjectDuplicator(SquareBox::PhysicsCollisionEngine::PhysicsWorld& m_physicsWorld, std::vector<SquareBox::GWOM::Layer>& layers_, std::vector<std::pair<int, int>>& m_selectedClusterObjects, SquareBox::GWOM::ClusterObject* m_currentClusterObject_ptr, SquareBox::IShape* m_currentShape_ptr, SquareBox::IMainGame* m_game, SquareBox::Utilities& m_utilities)
{
	std::pair<int, int> new_created_cluster_object_coordinates = std::pair<int, int>(-1, -1);//since our pointer is going to get iinvalidated
	//Duplicating ,for needs_width
	if (m_currentClusterObject_ptr != nullptr) {
		auto& current_layer = layers_[m_currentClusterObject_ptr->layer_index];
		if (
			m_game->getInputDevice()->isInputIdBeingReceived(cluster_object_selection_duplication_input_key_1)
			&&
			(
				m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)
				||
				m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)
				||
				m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)
				||
				m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)
				)

			) {
			if (m_currentShape_ptr->needs_height_width || m_currentShape_ptr->needs_radius) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				current_layer.world_clusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = current_layer.world_clusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject& dup = current_layer.world_clusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				dup.layer_index = orignals_cluster_index;
				m_utilities.nameClusterObjectByIndex(dup);

				if (m_currentShape_ptr->needs_height_width) {
					if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)) {
						dup.position.x -= orignals_properties_copy.width;
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)) {
						dup.position.x += orignals_properties_copy.width;
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)) {
						dup.position.y += orignals_properties_copy.height;
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)) {
						dup.position.y -= orignals_properties_copy.height;
					}
				}
				else {
					if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_west_duplication_input_key)) {
						dup.position.x -= (orignals_properties_copy.radius * 2);
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_east_duplication_input_key)) {
						dup.position.x += (orignals_properties_copy.radius * 2);
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_north_duplication_input_key)) {
						dup.position.y += (orignals_properties_copy.radius * 2);
					}
					else if (m_game->getInputDevice()->isInputIdReceived(cluster_object_selection_south_duplication_input_key)) {
						dup.position.y -= (orignals_properties_copy.radius * 2);
					}
				}

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, layers_, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (current_layer.world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(current_layer.alive_cluster_objects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						new_created_cluster_object_coordinates = details.settlementCoordinates;
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else {
				SBX_INFO("Shape duplicating for {} is  not yet implemented ", ToString(m_currentClusterObject_ptr->shape));
			}
		}
	}
	return new_created_cluster_object_coordinates;
}

void SquareBoxEditor::Editor_Assistant::cameraControls(SquareBox::Camera::ParallelCamera& camera_, SquareBox::IMainGame* gamePtr_)
{

	float calculated_movement_speed = 1 / camera_.getScale();

	//camera panning
	if (
		(
			gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_1_of_or_input_key)
			||
			gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_2_of_or_input_key)
			)
		&&
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_master_input_key)
		) {

		auto relation_motion = gamePtr_->getInputDevice()->getScreenLocations()[1].coordinates;
		camera_.offsetPosition(glm::vec2(-relation_motion.x, relation_motion.y * camera_.getAspectRatio()) * calculated_movement_speed);
		return;
	}
	//zoom
	if (
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_zooming_1_of_or_input_key)
		||
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_zooming_2_of_or_input_key)
		) {
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float scale_offset = mouse_wheel.y;
		if (std::abs(scale_offset)) {
			glm::vec2 mouse_in_world_before = camera_.convertScreenToWorld(gamePtr_->getInputDevice()->getScreenLocations()[0].coordinates);
			camera_.offsetScale(scale_offset);
			glm::vec2 mouse_in_world_after = camera_.convertScreenToWorld(gamePtr_->getInputDevice()->getScreenLocations()[0].coordinates);

			glm::vec2 dist_vec = mouse_in_world_before - mouse_in_world_after;
			camera_.offsetPosition(dist_vec);
			return;
		}
	}

	if (
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_motion_1_of_or_input_key)
		||
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_motion_2_of_or_input_key)
		) {
		//check for movement in x axis first
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float offset_in_x = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(offset_in_x, 0));
	}
	else {
		//all movement is in the y axis
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float offset_in_y = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(0, offset_in_y));
	}
}