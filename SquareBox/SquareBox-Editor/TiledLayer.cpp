#include "TiledLayer.h"

TiledLayer::TiledLayer()
{
}

TiledLayer::~TiledLayer()
{
}

void TiledLayer::onInit()
{
	layer_type = SquareBox::LayerTypeEnum::TiledLayer;
	m_utilities.init();
}

void TiledLayer::onLayerInit(SquareBox::GWOM::Layer& layer_)
{
	if (m_layer_width > 0 && m_layer_height > 0 && m_layer_tile_size > 0) {
		layer_.tile_system.init(0 - m_layer_width * 0.5f, 0 - m_layer_height * 0.5f, m_layer_width, m_layer_height, m_layer_tile_size);
		layer_.layer_type = SquareBox::LayerTypeEnum::TiledLayer;
	}
	else {
		SBX_ERROR("Invalid Tiling Parameters");
	}
}

int TiledLayer::onLayerInitGui(SquareBox::GWOM::Layer& layer_)
{
	ImGui::OpenPopup("Change Layer Type");
	// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	int choice = -1;
	if (ImGui::BeginPopupModal("Change Layer Type", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("All contents of this layer will be lost");
		ImGui::Separator();

		ImGui::Text("Layer Width   :"); ImGui::SameLine();
		ImGui::InputFloat("###layerwidth", &m_layer_width);

		ImGui::Text("Layer Height  :"); ImGui::SameLine();
		ImGui::InputFloat("###layerheight", &m_layer_height);

		float layer_x_origin = 0 - m_layer_width * 0.5f;
		float layer_y_origin = 0 - m_layer_height * 0.5f;
		ImGui::Text("Layer Origin  :  %.3f  ,%.3f", layer_x_origin, layer_y_origin);

		ImGui::Text("Tile Size     :"); ImGui::SameLine();
		ImGui::InputFloat("###layertileszie", &m_layer_tile_size);

		if (ImGui::Button("Ok", ImVec2(120, 0))) {
			if (m_layer_width > 0 && m_layer_height > 0 && m_layer_tile_size > 0) {

				choice = 1;
			}
			else {
				choice = 0;
			}
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			choice = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return choice;

}

void TiledLayer::onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{
}

void TiledLayer::onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{
}

void TiledLayer::onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_)
{
	m_selected_tiles.clear();
	m_active_sub_texture_key = 0;
}

void TiledLayer::onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_)
{
	active_layer_.index = new_layer_index_;
}

void TiledLayer::onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];

	//where the mouse currently is at in our world
	glm::vec2& mouse_in_world = active_layer.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

	if (editor_mode_ == EditorModeEnum::SELECT) {
		//figure out if we are in the tile system
		if (active_layer.tile_system.isInTileSystem(mouse_in_world)) {

			//get the tile being hovered
			if (game_ptr_->getInputDevice()->isInputIdReceived(tiled_layer_select_mode_select_tile)) {
				SquareBox::GWOM::Tile* selected_tile = active_layer.tile_system.getTile(mouse_in_world);
				if (selected_tile->key != -1) {
					std::pair<int, int> selected_tile_coordinates = std::pair<int, int>(active_layer.index, selected_tile->index);
					if (m_tiled_layer_selection_mode_index == TiledLayerSelectModeEnum::SINGLETILE) {
						m_selected_tiles.clear();
						m_utilities.addPairToVector(m_selected_tiles, selected_tile_coordinates);
					}
					else if (m_tiled_layer_selection_mode_index == TiledLayerSelectModeEnum::MULTIPLETILES) {
						if (m_utilities.isPairVectorMember(m_selected_tiles, selected_tile_coordinates)) {
							m_utilities.removePairFromVector(m_selected_tiles, selected_tile_coordinates);
						}
						else {
							m_utilities.addPairToVector(m_selected_tiles, selected_tile_coordinates);
						}
					}
				}
			}

			// deleting tiles
			/*
			 since we are deleting a tile according to its stored attributes ,
			 this function has the ability to delete tiles that are on other layers
			 as long as they are among the selected tiles
			*/
			if (game_ptr_->getInputDevice()->isInputIdReceived(delete_selection_input_key)) {
				for (unsigned int i = 0; i < m_selected_tiles.size(); i++)
				{
					SquareBox::GWOM::Tile* focus_tile = active_layer.tile_system.getTileByIndex(m_selected_tiles[i].second);
					focus_tile->key = -1;
					m_utilities.removePairFromVector(active_layer.tile_system.active_tiles, focus_tile->coordinates);
				}
				m_selected_tiles.clear();
			}
			//duplicating the selected tile
			m_editor_assitant.currentTileDuplicator(active_layer, m_selected_tiles, game_ptr_, m_utilities);
		}

	}
	else if (editor_mode_ == EditorModeEnum::PLACE) {
		if (active_layer.tile_system.isInTileSystem(mouse_in_world)) {
			SquareBox::GWOM::Tile* hovered_tile = active_layer.tile_system.getTile(mouse_in_world);
			if (game_ptr_->getInputDevice()->isInputIdReceived(tiled_layer_place_mode_fill_tile)) {
				//we have to edit the hovered tile and give it a texture to display
				hovered_tile->key = m_active_sub_texture_key;
				m_utilities.addPairToVector(active_layer.tile_system.active_tiles, hovered_tile->coordinates, false);
				m_is_all_work_saved = false;
			}
		}
	}
}

void TiledLayer::onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
}

void TiledLayer::onContentDraw(SquareBox::GWOM::Layer& active_layer, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
	//fix up the draw calls
	glm::vec2 layer_camera_center = active_layer.camera.getPosition();
	float layer_camera_width = active_layer.camera.getCameraDimensions().x;
	float layer_camera_height = active_layer.camera.getCameraDimensions().y;
	glm::vec4 camera_destRect(layer_camera_center - glm::vec2(layer_camera_width, layer_camera_height) * 0.5f, glm::vec2(layer_camera_width, layer_camera_height));
	std::map<int, SquareBox::GWOM::Tile*> vector_of_visible_tiles = active_layer.tile_system.getAllTilesInDestRect(camera_destRect, true);

	for (auto it = vector_of_visible_tiles.begin(); it != vector_of_visible_tiles.end(); it++)
	{
		if ((*it).second->key != -1) {
			auto specific_texturing_details = active_layer.getTextureIdAndUvReactFromKey((*it).second->key);
			glm::vec4 texture_uvRect = specific_texturing_details.second;
			int texture_id = specific_texturing_details.first;
			//we need ot figure these out using the key that this tile has 
			sprite_batch_.draw(glm::vec4((*it).second->position - glm::vec2(active_layer.tile_system.getTileSize()) * 0.5f, glm::vec2(active_layer.tile_system.getTileSize())), texture_uvRect, texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * (active_layer.opacity * 0.01)));
		}
	}
}

void TiledLayer::onDebugDraw(SquareBox::GWOM::Layer& active_layer, SquareBox::RenderEngine::DebugRenderer& debug_renderer_)
{
	//fix up the draw calls
	glm::vec2 layer_camera_center = active_layer.camera.getPosition();
	float layer_camera_width = active_layer.camera.getCameraDimensions().x;
	float layer_camera_height = active_layer.camera.getCameraDimensions().y;
	glm::vec4 camera_destRect(layer_camera_center - glm::vec2(layer_camera_width, layer_camera_height) * 0.5f, glm::vec2(layer_camera_width, layer_camera_height));
	std::map<int, SquareBox::GWOM::Tile*> vector_of_visible_tiles = active_layer.tile_system.getAllTilesInDestRect(camera_destRect, false);

	for (auto it = vector_of_visible_tiles.begin(); it != vector_of_visible_tiles.end(); it++)
	{
		debug_renderer_.drawBox(glm::vec4((*it).second->position - glm::vec2(active_layer.tile_system.getTileSize()) * 0.5f, glm::vec2(active_layer.tile_system.getTileSize())), m_layer_grid_color, 0.0f);
	}
}

void TiledLayer::onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_, EditorModeEnum editor_mode_)
{
	if (editor_mode_ == EditorModeEnum::PLACE) {
		glm::vec2 mouse_in_world = layer_.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

		if (layer_.tile_system.isInTileSystem(mouse_in_world)) {
			auto hovered_tile = layer_.tile_system.getTile(mouse_in_world);
			debug_renderer_.drawBox(glm::vec4(hovered_tile->position - glm::vec2(layer_.tile_system.getTileSize()) * 0.5f, glm::vec2(layer_.tile_system.getTileSize())), m_place_mode_sketch_color, 0.0f);
		}
	}
	else if (editor_mode_ == EditorModeEnum::SELECT) {
		for (auto it = m_selected_tiles.begin(); it != m_selected_tiles.end(); it++)
		{
			if (layer_.index == (*it).first) {
				//since the selected objects might not be from this layer
				SquareBox::GWOM::Tile* focus_tile = layer_.tile_system.getTileByIndex((*it).second);
				debug_renderer_.drawBox(glm::vec4(focus_tile->position - glm::vec2(layer_.tile_system.getTileSize()) * 0.5f, glm::vec2(layer_.tile_system.getTileSize())), m_selected_body_color, 0.0f);
			}
		}
	}
}

void TiledLayer::onLightDraw(SquareBox::GWOM::Layer& active_layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
}

void TiledLayer::onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];
	ImGui::Checkbox("Show grid lines", &m_show_grid_lines);

	if (ImGui::Button("Update Tiling")) {
		SBX_ERROR("Updating the nature of tiling is not yet implemented");
	}

	if (m_show_shift_tiles_to_another_layer_dialog) {
		ImGui::OpenPopup("ShiftTilesToAnotherLayer?");
	}


	ImGui::Text("Shape     : "); ImGui::SameLine();
	ImGui::Text(ToString(SquareBox::BodyShapeEnum::Box));
	ImGui::Text("Height    : "); ImGui::SameLine();
	ImGui::Text("%.3f", active_layer.tile_system.getTileSize());
	ImGui::Text("Width     : "); ImGui::SameLine();
	ImGui::Text("%.3f", active_layer.tile_system.getTileSize());

	if (ImGui::BeginPopupModal("ShiftTilesToAnotherLayer?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Shifting cluster Objects\nFrom: %s \n", active_layer.name);
		ImGui::Separator();

		ImGui::DragInt("New Layer index", &m_target_shift_to_layer_index, 1, 0, layers_.size() - 1, "%d%", ImGuiSliderFlags_AlwaysClamp);
		if (m_target_shift_to_layer_index < 0) {
			m_target_shift_to_layer_index = 0;
		}
		ImGui::Text("To: %s", layers_[m_target_shift_to_layer_index].name);

		if (active_layer.tile_system.isInitialised() && layers_[m_target_shift_to_layer_index].tile_system.isInitialised()) {
			auto& destination_layer = layers_[m_target_shift_to_layer_index];
			if (active_layer.layer_type != destination_layer.layer_type) {

				if (
					(SquareBox::MathLib::Float::areFloatsEqual(active_layer.tile_system.getOriginX(), destination_layer.tile_system.getOriginX()))
					&&
					(SquareBox::MathLib::Float::areFloatsEqual(active_layer.tile_system.getOriginY(), destination_layer.tile_system.getOriginY()))
					&&
					(SquareBox::MathLib::Float::areFloatsEqual(active_layer.tile_system.getHeight(), destination_layer.tile_system.getHeight()))
					&&
					(SquareBox::MathLib::Float::areFloatsEqual(active_layer.tile_system.getWidth(), destination_layer.tile_system.getWidth()))
					&&
					(SquareBox::MathLib::Float::areFloatsEqual(active_layer.tile_system.getTileSize(), destination_layer.tile_system.getTileSize()))
					) {
					/*
					different layers can be having different texture arrangements
					for this to work we shall need to make sure we have the same texture avaliable in the destination
					as it is in the orign layer , if it is not we shall have to copy it to the destination layer
					 the wway we are doing it right now where we just let the key pass through without cross checking
					 it , can lead to errors

					*/
					ImGui::Text("Not yet implementated, check comment in code {} {} ", __FILE__, __LINE__);
					bool enable_without_texture_sorting = false;
					if (enable_without_texture_sorting) {
						if (ImGui::Button("OK", ImVec2(120, 0))) {
							for (unsigned int i = 0; i < m_selected_tiles.size(); i++)
							{
								SquareBox::GWOM::Tile* orign_grid_tile = layers_[m_selected_tiles[i].first].tile_system.getTileByIndex(m_selected_tiles[i].second);
								//take to the target layer
								SquareBox::GWOM::Tile* destination_grid_tile = layers_[m_target_shift_to_layer_index].tile_system.getTile(orign_grid_tile->coordinates.first, orign_grid_tile->coordinates.second);
								destination_grid_tile->key = orign_grid_tile->key; // wrong !!! and can crush the application if the 
								// texture key is out of range of the layers textures
								m_utilities.addPairToVector(layers_[m_target_shift_to_layer_index].tile_system.active_tiles, destination_grid_tile->coordinates);


								//remove from the current layer
								orign_grid_tile->key = -1;
								m_utilities.removePairFromVector(active_layer.tile_system.active_tiles, orign_grid_tile->coordinates);
							}
							m_selected_tiles.clear(); // since object might have been transfered to a locked layer
							m_show_shift_tiles_to_another_layer_dialog = false;
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					if (ImGui::Button(enable_without_texture_sorting ? "Cancel" : "Okay", ImVec2(120, 0))) {
						m_show_shift_tiles_to_another_layer_dialog = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			else {
				ImGui::Text("Both layers must have the same tilling orientation");
			}
		}

	}

	int lsme = static_cast<int>(m_tiled_layer_selection_mode_index);
	TiledLayerSelectModeEnum beforelayerSelectionMode = m_tiled_layer_selection_mode_index;
	ImGui::Separator();
	ImGui::RadioButton("Single Tile", &lsme, 0); ImGui::SameLine();
	ImGui::RadioButton("Multiple Tiles", &lsme, 1);
	m_tiled_layer_selection_mode_index = static_cast<TiledLayerSelectModeEnum>(lsme);
	if (beforelayerSelectionMode != m_tiled_layer_selection_mode_index) {
		m_selected_tiles.clear();
	}
	if (m_tiled_layer_selection_mode_index == TiledLayerSelectModeEnum::MULTIPLETILES && m_selected_tiles.size() > 0) {
		if (ImGui::Button("Clear Selection")) {
			m_selected_tiles.clear();
		}
	}

	ImGui::Text("Texture");
	ImGui::Separator();
	static int current_texture_type = 0;
	static int m_selected_single_texture_index = 0;
	static int m_selected_tile_sheet_texture_index = 0;
	static int selected_tiling_index = 0;
	ImGui::RadioButton("Single Sprite", &current_texture_type, 0); ImGui::SameLine();
	ImGui::RadioButton("Tile Sheet", &current_texture_type, 1);

	SquareBox::TextureEnum selected_texture_type = SquareBox::TextureEnum::SINGLE;

	if (current_texture_type == 0) {
		selected_texture_type = SquareBox::TextureEnum::SINGLE;
	}
	else if (current_texture_type == 1) {
		selected_texture_type = SquareBox::TextureEnum::TILESHEET;
	}

	if (selected_texture_type == SquareBox::TextureEnum::SINGLE) {
		std::vector<char*>  vc;
		vc.reserve(active_layer.single_textures.size());
		std::transform(active_layer.single_textures.begin(), active_layer.single_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
		{
			//this new here is never deleted, and is called alot, that is a problem to us 
			char* pc = new char[parentTexture_.texture.display_name.size() + 1];
			std::strcpy(pc, parentTexture_.texture.display_name.c_str());
			return pc;
		});
		//compute how many single textures we want to display
		//we are only displaying those that are not empty strings
		if (active_layer.single_textures.size() > 0) {
			ImGui::Combo("Single", &m_selected_single_texture_index, &vc[0], active_layer.single_textures.size());

			//current active texture
			ImTextureID my_tex_id;
			my_tex_id = (ImTextureID)active_layer.single_textures[m_selected_single_texture_index].texture.id;

			float my_tex_w = 50;
			float my_tex_h = 50;
			float zoom = 4.0f;
			ImGui::Text("Preview");
			//get the uvCoords
			glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

			ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);                 // Top-left
			ImVec2 uv_max = ImVec2(uvRect.z, uvRect.w);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
			ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv_min, uv_max, tint_col, border_col);


			//set the m_active_sub_texture_key
			for (unsigned int i = 0; i < active_layer.sub_textures_table.size(); i++) {
				const auto& sub_texture = active_layer.sub_textures_table[i];
				if (
					(sub_texture.parent_texture_index == active_layer.single_textures[m_selected_single_texture_index].texture_index)
					&&
					(sub_texture.parent_type == SquareBox::TextureEnum::SINGLE)
					) {
					m_active_sub_texture_key = sub_texture.sub_texture_key;
				}
			}


		}
		else {
			ImGui::Combo("Single", &m_selected_single_texture_index, "Empty", active_layer.single_textures.size());
		}


	}
	else if (selected_texture_type == SquareBox::TextureEnum::TILESHEET) {
		if (active_layer.tiled_textures.size() > 0) {
			ImGui::Text("Tiling:");
			ImGui::Text("cols : "); ImGui::SameLine();
			ImGui::Text("%d", active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.x);
			ImGui::Text("rows : "); ImGui::SameLine();
			ImGui::Text("%d", active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.y);

			std::vector<char*>  vc;
			vc.reserve(active_layer.tiled_textures.size());
			std::transform(active_layer.tiled_textures.begin(), active_layer.tiled_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
			{
				//this new here is never deleted, and is called alot, that is a problem to us 
				char* pc = new char[parentTexture_.texture.display_name.size() + 1];
				std::strcpy(pc, parentTexture_.texture.display_name.c_str());
				return pc;
			});

			ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], active_layer.tiled_textures.size());
			//update the current objects texture info
			auto& selected_tile_sheet_texture = active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture;
			//current active texture
			ImTextureID my_tex_id;
			float my_tex_w = 0.0f;
			float my_tex_h = 0.0f;
			my_tex_id = (ImTextureID)selected_tile_sheet_texture.id;
			if (selected_tile_sheet_texture.tiling.x < 1 && selected_tile_sheet_texture.tiling.y < 1) {
				//the tilesheet sheet hasn't yet been grided
				if (selected_tile_sheet_texture.width > selected_tile_sheet_texture.height) {
					my_tex_w = 75;
					my_tex_h = 30;
				}
				else {
					//give it a different aspect ratio
					my_tex_w = 55;
					my_tex_h = 100;
				}
			}
			else {
				my_tex_w = 50;
				my_tex_h = 50;
			}


			float zoom = 4.0f;
			ImGui::Text("Preview");

			/*
			we update the cluster objects uv coords here .if it is a freelance cluster Object
			*/
			glm::vec4 uvRect = selected_tile_sheet_texture.getUVReactAtIndex(selected_tiling_index);

			ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);                 // Top-left
			ImVec2 uv_max = ImVec2(uvRect.z, uvRect.w);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
			ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv_min, uv_max, tint_col, border_col);
			ImGui::Text("Index: "); ImGui::SameLine();
			//ImGui::InputInt("", &m_current_cluster_object_ptr->texture_info.tile_sheet_index);
			ImGui::DragInt("###m_current_tile_sheet_index", &selected_tiling_index, 1, 0, (selected_tile_sheet_texture.tiling.x * selected_tile_sheet_texture.tiling.y) - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (selected_tiling_index < 0) {
				selected_tiling_index = 0;
			}

			// set the m_active_sub_texture_key
			for (unsigned int i = 0; i < active_layer.sub_textures_table.size(); i++) {
				const auto& sub_texture = active_layer.sub_textures_table[i];
				if (
					(sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET)
					&&
					(sub_texture.parent_texture_index == active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture_index)
					&&
					(sub_texture.tiling_index == selected_tiling_index)
					) {

					m_active_sub_texture_key = sub_texture.sub_texture_key;
					break;
				}
			}
		}
		else {
			ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, "Empty", active_layer.tiled_textures.size());
		}
	}
}



void TiledLayer::onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	ImGui::Text("Layer Origin  :  %.3f  ,  %.3f", layers_[active_layer_index_].tile_system.getTileSystemOrigin().x, layers_[active_layer_index_].tile_system.getTileSystemOrigin().y);
	ImGui::Text("Layer Width   :  %.3f  ", layers_[active_layer_index_].tile_system.getWidth());
	ImGui::Text("Layer Height  :  %.3f  ", layers_[active_layer_index_].tile_system.getHeight());
	ImGui::Text("Tile size     :  %.3f", layers_[active_layer_index_].tile_system.getTileSize());
	int total_tiles = layers_[active_layer_index_].tile_system.getNumXCells() * layers_[active_layer_index_].tile_system.getNumYCells();
	ImGui::Text("Num Tiles     :  %d", total_tiles);
}

void TiledLayer::onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	if (m_selected_tiles.size() > 0) {
		if (ImGui::Button("Shift Selected Tiles")) {
			//we active a pop up that lets us shift all selected Objects to another layer
			m_show_shift_tiles_to_another_layer_dialog = true;
		}
	}
}

void TiledLayer::onDispose()
{
	m_utilities.dispose();
}

void TiledLayer::onLayerDispose(SquareBox::GWOM::Layer& layer_)
{
	layer_.tile_system.resetTileSystem();
	m_selected_tiles.clear();
}
