#include "GamePlayScreen.h"

int GamePlayScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayScreen::build()
{
}

void GamePlayScreen::onEntry()
{
	//initailising the GUI camera
	m_hud_gui_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_game_play_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	m_hud_gui_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.5f));
	m_game_play_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.5f));
	m_hud_gui_camera.setScale(1.0f);
	m_game_play_camera.setScale(1.0f);

	//Intialize our sprite Batch
	m_sprite_batch.init();

	//Intialize our debug Renderer
	m_debug_renderer.init();

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();

	//the debugProgram
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertexColor");
	m_debug_program.linkShaders();

	//game stats gui
	m_game_stats_gui_elements_coordinates[m_humans_text_gui_id] = -1;
	m_game_stats_gui_elements_coordinates[m_zombie_text_gui_id] = -1;
	m_game_stats_gui_elements_coordinates[m_bullets_text_gui_id] = -1;
	m_game_stats_gui_elements_coordinates[m_fps_text_gui_id] = -1;
	m_game_stats_huds_and_gui.init(m_game_ptr, m_game_stats_gui_elements_coordinates, "Assets/Guis/game stats gui.dat");


	//game play gui
	m_game_play_gui_elements_coordinates[m_direction_center_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_direction_body_gui_id] = -1;

	m_game_play_gui_elements_coordinates[m_fire_accessories_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_fire_gradient_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_fire_color_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_fire_shadow_gui_id] = -1;

	m_game_play_gui_elements_coordinates[m_move_accessories_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_move_gradient_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_move_color_gui_id] = -1;
	m_game_play_gui_elements_coordinates[m_move_shadow_gui_id] = -1;

	m_game_play_gui_elements_coordinates[m_pause_gui_id] = -1;

	m_game_mobile_controls_gui.init(m_game_ptr, m_game_play_gui_elements_coordinates, "Assets/Guis/game mobile controls.dat");

	//game pause gui
	m_game_pause_gui_elements_coordinates[m_continue_gui_id] = -1;
	m_game_pause_gui_elements_coordinates[m_main_menu_gui_id] = -1;

	m_game_pause_huds_and_gui.init(m_game_ptr, m_game_pause_gui_elements_coordinates, "Assets/Guis/pause screen gui.dat");


	m_game_over_gui_elements_coordinates[m_game_over_gui_id] = -1;
	m_game_over_gui_elements_coordinates[m_your_score_gui_id] = -1;
	m_game_over_gui_elements_coordinates[m_your_score_int_gui_id] = -1;
	m_game_over_gui_elements_coordinates[m_high_score_gui_id] = -1;
	m_game_over_gui_elements_coordinates[m_play_again_gui_id] = -1;
	m_game_over_gui_elements_coordinates[m_main_menu_go_gui_id] = -1;

	m_game_over_huds_and_gui.init(m_game_ptr, m_game_over_gui_elements_coordinates, "Assets/Guis/game over gui.dat");

	//initialize our game logic
	m_game_logic.init(m_game_ptr, &m_agents_collision_grid, m_layers, m_game_play_camera, m_player_coordinates);
	m_utilities.init();

	//only show what is inside the collision boundaries
	float collsion_grid_width = m_agents_collision_grid.getWidth();
	float collsion_grid_height = m_agents_collision_grid.getHeight();
	glm::vec4 collision_grid_destRect(m_agents_collision_grid.getGridOrigin(), collsion_grid_width, collsion_grid_height);
	
	m_game_play_camera.bindCameraPositionToDestRect(collision_grid_destRect);
	//m_game_play_camera.unbindCameraPositionToDestRect();
	
	m_game_play_camera.setScale(3.5f); 
	// investigate why some scales are causing issue when 
	//retrieving collions objects
	// to fix this we need to fix the way we determine which cells are in view, because the current
	// methods is not sufficient for big cells , and this is something that i saw in the tile engine as well
	//m_object_show_coordinates = true;
	//m_show_grid = true;
}

void GamePlayScreen::update(const float& deltaTime_)
{
	m_hud_gui_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_game_play_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	//update the particle engines
	for (unsigned int i = 0; i < m_layers.size(); i++)
	{
		SquareBox::GWOM::Layer& focus_layer = m_layers[i];
		//update the layers particle engines
		focus_layer.particle_engine.update(deltaTime_);
	}
	bool player_set_to_fire = false;
	bool move_player = false;
	bool use_mobile_controls = false;

	SquareBox::GWOM::ClusterObject& player_cluster_object = m_layers[m_player_coordinates.first].world_clusters[m_player_coordinates.second.first].cluster_objects[m_player_coordinates.second.second];
#ifdef SQB_PLATFORM_ANDROID
	use_mobile_controls = true;
#endif
	if (use_mobile_controls) {
		//leave them displayed and fix the center movements location ratio
		m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_direction_center_gui_id)->second).location_ratio = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_direction_body_gui_id)->second).location_ratio;
	}

	m_game_play_camera.setPosition(player_cluster_object.position);

	if (m_process_game_play_controls) {
		

		if (use_mobile_controls) {

			//controls independent updates
			m_game_mobile_controls_gui.update(m_hud_gui_camera);

			SquareBox::GWOM::GUIElement& gui_direction_center = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_direction_center_gui_id)->second);
			SquareBox::GWOM::GUIElement& gui_direction_body = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_direction_body_gui_id)->second);
			SquareBox::GWOM::GUIElement& gui_fire_button = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_fire_accessories_gui_id)->second);
			SquareBox::GWOM::GUIElement& gui_move_button = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_move_accessories_gui_id)->second);
			SquareBox::GWOM::GUIElement& gui_pause_button = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_pause_gui_id)->second);

			//player controls
			player_set_to_fire = (gui_fire_button.state == SquareBox::GUIElementStateEnum::ACTIVE);
			move_player = (gui_move_button.state == SquareBox::GUIElementStateEnum::ACTIVE);

			if (gui_direction_body.state == SquareBox::GUIElementStateEnum::ACTIVE) {

				const std::vector<SquareBox::InputManager::LocationDetails>& locations_of_pointers_in_world = m_game_ptr->getInputDevice()->getScreenLocations();

				for (unsigned int i = 0; i < locations_of_pointers_in_world.size(); i++)
				{
					glm::vec2 pointer_in_world = m_hud_gui_camera.convertScreenToWorld(locations_of_pointers_in_world[i].coordinates);
					if (m_game_mobile_controls_gui.isCoordinateInGUIElement(pointer_in_world, m_game_play_gui_elements_coordinates.find(m_direction_body_gui_id)->second)) {
						m_game_mobile_controls_gui.setGUIElementLocationRatio(pointer_in_world, m_game_play_gui_elements_coordinates.find(m_direction_center_gui_id)->second);
						player_cluster_object.direction = glm::normalize(pointer_in_world - m_game_mobile_controls_gui.getGuiElementWorldPositionByIndex(m_game_play_gui_elements_coordinates.find(m_direction_body_gui_id)->second));
					}
				}
			}

			//pausing
			if (gui_pause_button.state==SquareBox::GUIElementStateEnum::ACTIVE) {
					m_process_game_play_controls = false;
					m_process_game_pause_controls = true;

					m_display_game_play_controls = false;
					m_display_game_pause_controls = true;

					m_update_game_world = false;
					gui_pause_button.is_hidden = true;
			}
		}
		else {
			//moving
			if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::SPACE))) {
				move_player = true;
			}

			//firing
			if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
				player_set_to_fire = true;
			}

			//direction
			glm::vec2 mouse_in_world = m_game_play_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
			player_cluster_object.direction = glm::normalize(mouse_in_world - player_cluster_object.position);

			//pausing
			if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_p))) {

				m_process_game_play_controls = false;
				m_process_game_pause_controls = true;

				m_display_game_play_controls = false;
				m_display_game_pause_controls = true;

				m_update_game_world = false;
			}
		}

		if (move_player) {
			player_cluster_object.position += player_cluster_object.direction * player_cluster_object.speed * deltaTime_;
		}
	}

	if (m_process_game_pause_controls) {
		m_game_pause_huds_and_gui.update(m_hud_gui_camera);

		if (use_mobile_controls) {
			if (m_game_pause_huds_and_gui.getGuiElementByIndex(m_game_pause_gui_elements_coordinates.find(m_continue_gui_id)->second).state == SquareBox::GUIElementStateEnum::ACTIVE) {
				m_process_game_play_controls = true;
				m_process_game_pause_controls = false;
				m_display_game_play_controls = true;
				m_display_game_pause_controls = false;
				m_update_game_world = true;
				SquareBox::GWOM::GUIElement& gui_pause_button = m_game_mobile_controls_gui.getGuiElementByIndex(m_game_play_gui_elements_coordinates.find(m_pause_gui_id)->second);
				gui_pause_button.is_hidden = false;
			}
		}
		else {
			m_game_pause_huds_and_gui.getGuiElementByIndex(m_game_pause_gui_elements_coordinates.find(m_continue_gui_id)->second).is_hidden = true;//we use the 
			//keyboard to unpause
			if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_l))) {
				m_process_game_play_controls = true;
				m_process_game_pause_controls = false;

				m_display_game_play_controls = true;
				m_display_game_pause_controls = false;

				m_update_game_world = true;
			}
		}
	}

	//Game logic update
	if (m_update_game_world) {
		m_game_logic.update(deltaTime_, player_set_to_fire, m_layers);
	}

	if (m_process_game_stats) {
		m_game_stats_huds_and_gui.update(m_hud_gui_camera);
		//update our deplayed status
		sprintf(m_game_stats_huds_and_gui.getGuiElementByIndex(m_game_stats_gui_elements_coordinates.find(m_humans_text_gui_id)->second).fonts[static_cast<int>(SquareBox::GUIElementStateEnum::DEFAULT)].text, "HUMANS %d", m_layers[LayerIndicies::humans_layer_index].alive_cluster_objects.size());
		sprintf(m_game_stats_huds_and_gui.getGuiElementByIndex(m_game_stats_gui_elements_coordinates.find(m_zombie_text_gui_id)->second).fonts[static_cast<int>(SquareBox::GUIElementStateEnum::DEFAULT)].text, "ZOMBIES %d", m_layers[LayerIndicies::zombies_layer_index].alive_cluster_objects.size());
		sprintf(m_game_stats_huds_and_gui.getGuiElementByIndex(m_game_stats_gui_elements_coordinates.find(m_bullets_text_gui_id)->second).fonts[static_cast<int>(SquareBox::GUIElementStateEnum::DEFAULT)].text, "BULLETS %d", m_layers[LayerIndicies::ammunition_layer_index].alive_cluster_objects.size());
		sprintf(m_game_stats_huds_and_gui.getGuiElementByIndex(m_game_stats_gui_elements_coordinates.find(m_fps_text_gui_id)->second).fonts[static_cast<int>(SquareBox::GUIElementStateEnum::DEFAULT)].text, "FPS %d", m_game_ptr->getFps());
	}
	
	if (!player_cluster_object.is_alive) {
		m_process_game_play_controls = false;
		m_process_game_pause_controls = false;
		m_process_game_stats = false;
		m_process_game_over_controls = true;

		m_display_game_play_controls = false;
		m_display_game_pause_controls = false;
		m_display_game_stats = false;
		m_display_game_over_controls = true;

		m_game_over_huds_and_gui.update(m_hud_gui_camera);

		if (m_game_over_huds_and_gui.getGuiElementByIndex(m_game_over_gui_elements_coordinates.find(m_play_again_gui_id)->second).state==SquareBox::GUIElementStateEnum::ACTIVE) {
			//m_layers = m_layers_backup;
			//grid objects have to be readded to the grid
			
			m_game_logic.restart(m_layers, m_player_coordinates);

			m_display_game_stats = true;
			m_process_game_stats = true;

			m_process_game_play_controls = true;
			m_display_game_play_controls = true;

			m_display_game_over_controls = false;
			m_process_game_over_controls = false;
		}
	}
}

void GamePlayScreen::draw()
{

	// concerate layer
	SquareBox::GWOM::Layer& concerate_layer = m_layers[LayerIndicies::concerate_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, concerate_layer);


	// roads_layer
	SquareBox::GWOM::Layer& roads_layer = m_layers[LayerIndicies::roads_and_grass_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, roads_layer);


	// walls layer
	SquareBox::GWOM::Layer& bricks_layer = m_layers[LayerIndicies::bricks_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, bricks_layer);

	float camera_width = m_game_play_camera.getCameraDimensions().x;
	float camera_height = m_game_play_camera.getCameraDimensions().y;
	glm::vec4 camera_destRect(m_game_play_camera.getPosition() - (glm::vec2(camera_width, camera_height) * 0.5f), glm::vec2(camera_width, camera_height));


	// The contents of the collision grid
	std::map<int, SquareBox::GWOM::Cell*> collision_grid_cells_in_view = m_agents_collision_grid.getAllCellsInDestRect(camera_destRect);
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_game_play_camera, "P");
	m_sprite_batch.begin();
	for (auto it = collision_grid_cells_in_view.begin(); it != collision_grid_cells_in_view.end(); it++)
	{
		for (auto it_2 = (*it).second->member_cordinates.begin(); it_2 != (*it).second->member_cordinates.end(); it_2++)
		{
			std::pair<int, std::pair<int, int>> focus_cell_member_coordinates = (*it_2);
			SquareBox::GWOM::ClusterObject& focus_cluster_object = m_layers[focus_cell_member_coordinates.first].world_clusters[focus_cell_member_coordinates.second.first].cluster_objects[focus_cell_member_coordinates.second.second];
			//converting direction to angle
			const glm::vec2 right(1.0f, 0.0f);
			//now the dot product
			float angle = acos(glm::dot(right, focus_cluster_object.direction));
			//account for negative angle rotations
			if (focus_cluster_object.direction.y < 0.0f) angle = -angle;
			focus_cluster_object.angle = angle;
			m_utilities.setCurrentShapePointer(focus_cluster_object.shape, &m_current_shape_ptr);
			m_current_shape_ptr->draw(focus_cluster_object, m_sprite_batch, m_layers[focus_cell_member_coordinates.first].opacity);
		}
	}
	// the blood partciles
	m_layers[LayerIndicies::ammunition_layer_index].particle_engine.draw(&m_sprite_batch);

	m_sprite_batch.end();
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	// house tops layer
	SquareBox::GWOM::Layer& house_tops_layer = m_layers[LayerIndicies::house_tops_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, house_tops_layer);

	// trees layer
	SquareBox::GWOM::Layer& trees_layer = m_layers[LayerIndicies::trees_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, trees_layer);

	if (m_display_game_stats) {
		m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);
		m_game_stats_huds_and_gui.draw(m_hud_gui_camera, m_sprite_batch);
		m_sprite_batch.end();
		m_texture_program.use();
		preUpdateShader(&m_texture_program, "mySampler");
		uploadCameraInfo(&m_texture_program, &m_hud_gui_camera, "P");
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();
	}


	if (m_display_game_play_controls) {
		m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);
		m_game_mobile_controls_gui.draw(m_hud_gui_camera, m_sprite_batch);
		m_sprite_batch.end();
		m_texture_program.use();
		preUpdateShader(&m_texture_program, "mySampler");
		uploadCameraInfo(&m_texture_program, &m_hud_gui_camera, "P");
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();
	}

	if (m_display_game_pause_controls) {
		m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);
		m_game_pause_huds_and_gui.draw(m_hud_gui_camera, m_sprite_batch);
		m_sprite_batch.end();
		m_texture_program.use();
		preUpdateShader(&m_texture_program, "mySampler");
		uploadCameraInfo(&m_texture_program, &m_hud_gui_camera, "P");
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();
	}

	if (m_display_game_over_controls) {
		m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);
		m_game_over_huds_and_gui.draw(m_hud_gui_camera, m_sprite_batch);
		m_sprite_batch.end();
		m_texture_program.use();
		preUpdateShader(&m_texture_program, "mySampler");
		uploadCameraInfo(&m_texture_program, &m_hud_gui_camera, "P");
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();
	}
}

void GamePlayScreen::onExit()
{
	m_game_mobile_controls_gui.dispose();
	m_game_pause_huds_and_gui.dispose();
	m_game_stats_huds_and_gui.dispose();
	m_game_over_huds_and_gui.dispose();
}

void GamePlayScreen::destroy()
{
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();

	m_texture_program.dispose();
	m_debug_program.dispose();

	m_hud_gui_camera.dispose();
	m_game_play_camera.dispose();

	m_game_mobile_controls_gui.dispose();
	//m_game_logic.dispose();

	m_utilities.dispose();
}

void GamePlayScreen::drawLayer(SquareBox::RenderEngine::SpriteBatch& sprite_batch_, SquareBox::Camera::ParallelCamera& camera_, SquareBox::RenderEngine::GLSLProgram& texture_program_, SquareBox::GWOM::Layer& layer_)
{
	float camera_width = camera_.getCameraDimensions().x;
	float camera_height = camera_.getCameraDimensions().y;
	glm::vec4 camera_destRect(camera_.getPosition() - (glm::vec2(camera_width, camera_height) * 0.5f), glm::vec2(camera_width, camera_height));

	auto vector_of_visible_roof_tops_tiles = layer_.tile_system.getAllTilesInDestRect(camera_destRect, true);

	sprite_batch_.begin();

	for (auto it = vector_of_visible_roof_tops_tiles.begin(); it != vector_of_visible_roof_tops_tiles.end(); it++)
	{
		if ((*it).second->key != -1) {
			auto specific_texturing_details = layer_.getTextureIdAndUvReactFromKey((*it).second->key);
			glm::vec4 texture_uvRect = specific_texturing_details.second;
			int texture_id = specific_texturing_details.first;
			//we need ot figure these out using the key that this tile has 
			sprite_batch_.draw(glm::vec4((*it).second->position - glm::vec2(layer_.tile_system.getTileSize()) * 0.5f, glm::vec2(layer_.tile_system.getTileSize())), texture_uvRect, texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * (layer_.opacity * 0.01)));
		}
	}

	sprite_batch_.end();
	texture_program_.use();
	preUpdateShader(&texture_program_, "mySampler");
	uploadCameraInfo(&texture_program_, &camera_, "P");
	sprite_batch_.renderBatch();
	texture_program_.unuse();
}
