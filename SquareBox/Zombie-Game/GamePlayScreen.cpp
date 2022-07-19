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
	m_game_play_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() *0.5f, m_game_ptr->getWindow()->getScreenHeight() *0.5f));
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


	//initialize our games GUI
	m_game_huds_and_gui.init(m_game_ptr);

	//initialize our game logic
	m_game_logic.init(m_game_ptr, m_game_ptr->getAudioEngine(), m_game_ptr->getWindow(),&m_agents_collision_grid,m_layers,m_game_play_camera,m_player_coordinates);

	m_utilities.init();

	m_sprite_font.initWithName("Comfortaa-Regular.ttf", m_game_ptr->getWindow()->getScreenHeight() * 0.05f);
	
		auto music_raw_data = SquareBox::AssetManager::IOManager::getRawDataFromFile("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
		m_music_1 = SquareBox::AudioSystem::Music("", SquareBox::FallOffEnum::INVERSE, 10.0f, 50.0f, glm::vec2(0.0f), music_raw_data.first, music_raw_data.second);
		//lets load our music from here
		m_game_ptr->getAudioEngine()->loadMusic(m_music_1);
		m_music_1.play();
		auto raw_data = SquareBox::AssetManager::IOManager::getRawDataFromFile("Assets/Audio/rifle.wav");
		m_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("", raw_data.first, raw_data.second));

		m_game_ptr->getAudioEngine()->loadSoundBank(m_sound_bank);
		//m_sound_bank.play();

		auto sound_effect1 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
		auto sound_effect2 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/Patoranking-Love-You-Die-ft.-Diamond-Platnumz.mp3");
		auto sound_effect3 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/prettyboydo, Wani - Mentally (320).mp3");
		auto sound_effect4 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/Reminisce-ft-Fireboy-DML-Ogaranya.mp3");
		auto sound_effect5 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
		auto sound_effect6 = SquareBox::AudioSystem::SoundEffect("Assets/Audio/Tion Wayne feat. Afro B - Cant Go Broke.mp3");
		//m_sound_bank = SquareBox::AudioSystem::SoundBank(SquareBox::FallOffEnum::LINEAR, 5.0f, 30.0f, glm::vec2(0.0f));
		//m_sound_bank.sound_effects.push_back(sound_effect1);
		//m_sound_bank.sound_effects.push_back(sound_effect2);
		//m_sound_bank.sound_effects.push_back(sound_effect3);
		//m_sound_bank.sound_effects.push_back(sound_effect4);
		//m_sound_bank.sound_effects.push_back(sound_effect5);

		//m_audio_engine_ptr->loadSoundBank(m_sound_bank);
		//m_sound_bank.play();
	m_game_play_camera.setScale(3.5f);
	//m_object_show_coordinates = true;
	//m_show_grid = true;
}

void GamePlayScreen::update(const float & deltaTime_)
{
	/* 
	making the layers networking safe
	
	 we can not be adding in new cluster objects in our layers while we are iterating over them ,
	 doing so will result in us having memory access errors as the vector resizes
	 
	A networking safe option would be to keep the new cluster objects in limbo , and only add them in when
	we are not using  the layer system actively like right here
	
	
	*/
	



	// update the huds and GUI camera
	m_hud_gui_camera.update(m_game_ptr->getWindow()->getScreenWidth() ,m_game_ptr->getWindow()->getScreenHeight());
	m_game_play_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	//set the game play camera to always be at the players location

	//update the particle engines
	for (unsigned int i = 0; i < m_layers.size(); i++)
	{
		SquareBox::GWOM::Layer & focus_layer = m_layers[i];
		//update the layers particle engines
		focus_layer.particle_engine.update(deltaTime_);
	}
	SquareBox::GWOM::ClusterObject & player_cluster_object = m_layers[m_player_coordinates.first].world_clusters[m_player_coordinates.second.first].cluster_objects[m_player_coordinates.second.second];

	bool player_set_to_move = false;
	bool player_set_to_fire = false;
	glm::vec2 player_direction =player_cluster_object.direction; //since mobile player position is only set when he moves
	

	//huds and GUI update
	m_game_huds_and_gui.update(m_game_play_camera, player_cluster_object.position, player_set_to_move, player_direction, player_set_to_fire);

	if (!player_cluster_object.is_alive) {
		// never fire
		player_set_to_fire = false;
	}

	if (player_set_to_fire) {
		m_sound_bank.play();
	}
	//Game logic update
	m_game_logic.update(deltaTime_, player_set_to_fire, player_set_to_move, player_direction, m_layers);

	glm::vec2 new_camera_position = m_game_play_camera.getPosition();
	if (player_cluster_object.is_alive) {
		/*

			update our players movement and direction
			together with where in the world our camera is facing
		*/
		player_cluster_object.direction = player_direction;
		if (player_set_to_move) {
			player_cluster_object.position += player_cluster_object.direction * player_cluster_object.speed * deltaTime_;
			new_camera_position = player_cluster_object.position;
		}
	}
	else {
		//player is dead so just move camera around in the world
		//with respect to the dead players body
		player_cluster_object.direction = player_direction;
		if (player_set_to_move) {
			player_cluster_object.position += player_cluster_object.direction * player_cluster_object.speed * deltaTime_;
			new_camera_position = player_cluster_object.position;
		}
	}

	m_game_play_camera.setPosition(new_camera_position);
	m_game_ptr->getAudioEngine()->update(glm::vec2(0.0f));
}

void GamePlayScreen::draw()
{
	/*
		  since  according to the physics of this game
		  a grid can not hold two objects in the same area.
		  we are going to draw the game basing on the grid for the chase of objects that can be in a collision grid

		  those that can not will be drawn appropriatley
	*/
	char buffer[256];
	// draw world


	// concerate layer
	SquareBox::GWOM::Layer& concerate_layer = m_layers[LayerIndicies::concerate_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, concerate_layer);


	// roads_layer
	SquareBox::GWOM::Layer& roads_layer = m_layers[LayerIndicies::roads_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, roads_layer);


	// grass layer
	SquareBox::GWOM::Layer& grass_layer = m_layers[LayerIndicies::grass_layer_index];
	drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, grass_layer);


	//// walls layer
	//SquareBox::GWOM::Layer& bricks_layer = m_layers[LayerIndicies::bricks_layer_index];
	//drawLayer(m_sprite_batch, m_game_play_camera, m_texture_program, bricks_layer);

	float camera_width = m_game_play_camera.getCameraDimensions().x;
	float camera_height = m_game_play_camera.getCameraDimensions().y;
	glm::vec4 camera_destRect(m_game_play_camera.getPosition() - (glm::vec2(camera_width, camera_height)*0.5f), glm::vec2(camera_width, camera_height));


	// The contents of the collision grid
	std::map<int, SquareBox::GWOM::Cell*> collision_grid_cells_in_view = m_agents_collision_grid.getAllCellsInDestRect(camera_destRect);

	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_game_play_camera, "P");
	m_sprite_batch.begin();
	for (auto it = collision_grid_cells_in_view.begin(); it != collision_grid_cells_in_view.end(); it++)
	{
		int cell_members_count = 0;
		for (auto it_2 = (*it).second->member_cordinates.begin(); it_2 != (*it).second->member_cordinates.end();it_2++)
		{
			std::pair<int, std::pair<int, int>> focus_cell_member_coordinates = (*it_2);
			SquareBox::GWOM::ClusterObject & focus_cluster_object = m_layers[focus_cell_member_coordinates.first].world_clusters[focus_cell_member_coordinates.second.first].cluster_objects[focus_cell_member_coordinates.second.second];
			//converting direction to angle
			const glm::vec2 right(1.0f, 0.0f);
			//now the dot product
			float angle = acos(glm::dot(right, focus_cluster_object.direction));
			//account for negative angle rotations
			if (focus_cluster_object.direction.y < 0.0f) angle = -angle;
			focus_cluster_object.angle = angle;
			m_utilities.setCurrentShapePointer(focus_cluster_object.shape, &m_current_shape_ptr);
			m_current_shape_ptr->draw(focus_cluster_object, m_sprite_batch, m_layers[focus_cell_member_coordinates.first].opacity);
			if (m_object_show_coordinates) {
				int cell_index = (*it).first;
				sprintf(buffer, "%d %d", cell_index, cell_members_count);
				m_sprite_font.draw(m_sprite_batch, buffer, focus_cluster_object.position, glm::vec2((1 / m_game_play_camera.getScale())*0.3f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green), SquareBox::JustificationEnum::MIDDLE);
			}

			cell_members_count++;
		}
		if (m_object_show_coordinates) {
			int cell_index = (*it).first;
			sprintf(buffer, "%d %d", cell_index, (*it).second->index);
			m_sprite_font.draw(m_sprite_batch, buffer, (*it).second->position, glm::vec2((1 / m_game_play_camera.getScale())*0.4f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow), SquareBox::JustificationEnum::MIDDLE);
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

	//draw the huds and gui
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_game_play_camera, "P");
	m_sprite_batch.begin(SquareBox::RenderEngine::NONE);
	m_game_huds_and_gui.draw(m_sprite_batch,m_sprite_font, m_layers);
	m_sprite_batch.end();
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	if (m_show_grid) {
		m_debug_program.use();
		uploadCameraInfo(&m_debug_program, &m_game_play_camera, "P");
		m_debug_renderer.begin();
		m_debug_renderer.drawBox(camera_destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green), 0.0f);

		//draw the collision cells in camera view
		for (auto it = collision_grid_cells_in_view.begin(); it != collision_grid_cells_in_view.end(); it++)
		{
			glm::vec2 grid_cell_position = (*it).second->position;
			float grid_cell_size = m_agents_collision_grid.getCellSize();
			glm::vec4 grid_cell_destRect(grid_cell_position - glm::vec2(grid_cell_size)*0.5f, glm::vec2(grid_cell_size));
			m_debug_renderer.drawBox(grid_cell_destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);

		}
		m_debug_renderer.end();
		m_debug_renderer.render();
		m_debug_program.unuse();
	}
}

void GamePlayScreen::onExit()
{
}

void GamePlayScreen::destroy()
{
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();

	m_texture_program.dispose();
	m_debug_program.dispose();

	m_hud_gui_camera.dispose();
	m_game_play_camera.dispose();

	m_game_huds_and_gui.dispose();
	m_game_logic.dispose();

	m_utilities.dispose();

	m_sprite_font.dispose();
}

void GamePlayScreen::drawLayer(SquareBox::RenderEngine::SpriteBatch& sprite_batch_, SquareBox::Camera::ParallelCamera& camera_, SquareBox::RenderEngine::GLSLProgram& texture_program_,SquareBox::GWOM::Layer & layer_)
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
