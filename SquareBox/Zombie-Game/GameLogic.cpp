#include "GameLogic.h"

void GameLogic::init(SquareBox::IMainGame * game_ptr_, SquareBox::AudioSystem::AudioEngine * audio_engine_ptr, SquareBox::RenderEngine::Window * window_ptr_, SquareBox::GWOM::Grid * collision_grid_ptr_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::Camera::ParallelCamera & game_play_camera_, std::pair<int, std::pair<int, int>> & player_coordinates_)
{
	//adopt all the initialisation variables
	m_game_ptr = game_ptr_;
	m_audio_device_ptr = audio_engine_ptr;
	m_window_ptr = window_ptr_;
	m_collision_grid_ptr = collision_grid_ptr_;
	m_utilities.init();
	loadLevel("Assets/Levels/Zombie Level v4.dat", layers_,game_play_camera_,player_coordinates_, true);
	m_human.init();
	m_player.init();
	m_zombie.init();
	auto bullets_texture = SquareBox::AssetManager::TextureManager::getTextureByName("circle.png", window_ptr_->getDDPI());
	auto blood_texture = SquareBox::AssetManager::TextureManager::getTextureByName("particle.png", window_ptr_->getDDPI());
	const float bullet_speed = 4.0f;
	const float bullet_size = 0.8f;
	const float bullet_damage = 5.0f;
	const int blood_partcles = 50;
	const float blood_particles_width = 1.0f;
	const float bullet_decay_rate = 0.1f;
	m_bullet.init(bullets_texture, bullet_size, bullet_speed,bullet_damage, bullet_decay_rate, blood_partcles,blood_particles_width);

	//set up the ammunition layers blood_particle_batch

	m_blood_particle_batch_ptr = new SquareBox::RenderEngine::ParticleBatch;

	m_blood_particle_batch_ptr->init(5000, 0.1f, blood_texture, [](SquareBox::RenderEngine::Particle& particle_, float delta_time) {
		particle_.position += particle_.velocity * delta_time;
		particle_.color.a = (GLubyte)(particle_.life * 255.0f);
		});
	layers_[LayerIndicies::ammunition_layer_index].particle_engine.addParticleBatch(m_blood_particle_batch_ptr);
	SquareBox::AudioSystem::SoundBank gun_sound_bank;
	m_gun.init("AK 47",1,10,0.1f,gun_sound_bank,&m_bullet);
}

void GameLogic::update(float delta_time_, bool fire_, bool move_player_, glm::vec2 player_direction_, std::vector<SquareBox::GWOM::Layer>& layers_)
{
	SquareBox::GWOM::ClusterObject& player_cluster_object = layers_[m_player_coordinates.first].world_clusters[m_player_coordinates.second.first].cluster_objects[m_player_coordinates.second.second];

	m_gun.update(delta_time_, fire_, player_cluster_object.position, player_direction_, layers_, m_collision_grid_ptr);

	for (int y = 0; y < m_collision_grid_ptr->getNumYCells(); y++)
	{
		for (int x = 0; x < m_collision_grid_ptr->getNumXCells(); x++)
		{
			SquareBox::GWOM::Cell* current_cell = m_collision_grid_ptr->getCell(x,y);
			for (unsigned int i = 0; i < current_cell->member_cordinates.size(); i++)
			{
				//check collision with the other cell members
				checkCollisionCellCollision(delta_time_, layers_, current_cell->member_cordinates[i], current_cell->index, i + 1);
			}
		}
	}

	//for (size_t i = 0; i < total_cells; i++) {

	//	int x = i % m_collision_grid_ptr->getNumXCells();
	//	int y = i / m_collision_grid_ptr->getNumXCells();


	//	// Loop through all balls in a cell
	//	for (size_t j = 0; j < current_cell->member_cordinates.size(); j++) {
	//		std::pair<int, std::pair<int, int>> & current_cell_member_coordinates = current_cell->member_cordinates[j];

	//		//check collision with the current cell
	//		/// Update with the residing cell
	//		checkCollisionCellCollision(delta_time_, layers_, current_cell_member_coordinates,current_cell->index, j+1);

	//		/// Update collision with neighbor cells
	//		if (x > 0) {
	//			// Left
	//			checkCollisionCellCollision(delta_time_, layers_, current_cell_member_coordinates, m_collision_grid_ptr->getCell(x - 1, y)->index, 0);

	//			if (y > 0) {
	//				/// Top left
	//				checkCollisionCellCollision(delta_time_, layers_, current_cell_member_coordinates, m_collision_grid_ptr->getCell(x - 1, y - 1)->index, 0);

	//			}
	//			if (y < m_collision_grid_ptr->getNumYCells() - 1) {
	//				// Bottom left
	//				checkCollisionCellCollision(delta_time_, layers_, current_cell_member_coordinates, m_collision_grid_ptr->getCell(x - 1, y + 1)->index, 0);

	//			}
	//		}
	//		// Up cell
	//		if (y > 0) {
	//			checkCollisionCellCollision(delta_time_, layers_, current_cell_member_coordinates, m_collision_grid_ptr->getCell(x, y - 1)->index, 0);
	//		}
	//	}
	//}
}



void GameLogic::dispose()
{
	m_human.dispose();
	m_player.dispose();
	m_zombie.dispose();
	m_bullet.dispose();
	m_gun.dispose();
	m_collision_grid_ptr->dispose();
	m_bullet.dispose();
	m_utilities.dispose();
}

void GameLogic::checkCollisionCellCollision(float delta_time_, std::vector<SquareBox::GWOM::Layer>& layers_,std::pair<int, std::pair<int, int>> cluster_object_full_coordinates_, int grid_index_with_collision_members_, int start_index_)
{
	SquareBox::GWOM::ClusterObject & cluster_object = layers_[cluster_object_full_coordinates_.first].world_clusters[cluster_object_full_coordinates_.second.first].cluster_objects[cluster_object_full_coordinates_.second.second];
	if (cluster_object.is_alive)
	{
		m_collision_agent_ptr = nullptr;
		if (cluster_object.layer_index == LayerIndicies::humans_layer_index) {
			cluster_object_full_coordinates_.first;
			cluster_object_full_coordinates_.second.first;
			cluster_object_full_coordinates_.second.second;
			if (cluster_object_full_coordinates_== m_player_coordinates) {
				m_collision_agent_ptr = &m_player;
			}
			else {
				m_collision_agent_ptr = &m_human;
			}
		}
		else if (cluster_object.layer_index == LayerIndicies::zombies_layer_index) {
			m_collision_agent_ptr = &m_zombie;
		}
		else if (cluster_object.layer_index == LayerIndicies::ammunition_layer_index) {
			m_bullet.update(delta_time_,m_player_coordinates, cluster_object_full_coordinates_, layers_, grid_index_with_collision_members_, m_blood_particle_batch_ptr, m_collision_grid_ptr);
		}
		else {
			SBX_CRITICAL("unidentified layer {} ", layers_[cluster_object.layer_index].name);
		}

		if (m_collision_agent_ptr != nullptr) {
			// collision agent ptr will be null for ammunition layer 
			m_collision_agent_ptr->update(delta_time_, cluster_object_full_coordinates_,layers_, grid_index_with_collision_members_, start_index_, m_blood_particle_batch_ptr, m_collision_grid_ptr);
		}
		
		//refresh the cluster object
		SquareBox::GWOM::ClusterObject & refreshed_cluster_object = layers_[cluster_object_full_coordinates_.first].world_clusters[cluster_object_full_coordinates_.second.first].cluster_objects[cluster_object_full_coordinates_.second.second];
		if (refreshed_cluster_object.is_alive) {
			m_collision_grid_ptr->updateObjectGridLocation(refreshed_cluster_object);
		}
	}
}

void GameLogic::loadLevel(const char * file_path_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::Camera::ParallelCamera & game_play_camera_, std::pair<int, std::pair<int, int>> & player_coordinates_, bool first_empty_world_)
{
	if (first_empty_world_) {
		layers_.clear();
		m_collision_grid_ptr->resetGrid();
		m_collision_grid_ptr->init(-540, -540, 1080, 1080, 200);
	}
	SquareBox::GWOM::LevelReaderWriter level_loader;
	int active_layer_index = 0;
	float active_camera_scale;
	glm::vec2 active_camera_position;
	level_loader.loadLevelDataAsBinary(std::string(file_path_), layers_,active_camera_scale,active_camera_position,active_layer_index);
	game_play_camera_.setScale(active_camera_scale);
	game_play_camera_.setPosition(active_camera_position);
	bool found_player_coordinates = false;
	for (unsigned int i = 0; i < layers_.size(); i++)
	{
		found_player_coordinates = false; // only turns true when we are no the players layer
		SquareBox::GWOM::Layer & focus_layer = layers_[i];
		//load the layers textures
		m_utilities.loadLayerTextures(focus_layer, m_window_ptr->getDDPI());

		if (focus_layer.tile_system.isInitialised()) {
			/*
				we need a world cluster in layers where we are going to store all the tiles that have been
				converted to cluster objects
			*/

			// the conversion
			m_collision_agent_ptr = nullptr;
			int target_layer_world_cluster = -1;
			bool discard_tile_system = false;


			if (
				(std::string(focus_layer.name) == LayerIndicies::concerate_layer_name)
				||
				(std::string(focus_layer.name) == LayerIndicies::roads_layer_name)
				||
				(std::string(focus_layer.name) == LayerIndicies::grass_layer_name)
				||
				(std::string(focus_layer.name) == LayerIndicies::house_tops_layer_name)
				||
				(std::string(focus_layer.name) == LayerIndicies::trees_layer_name)
				||
				(std::string(focus_layer.name) == LayerIndicies::bricks_layer_name)
				)
			{
				/* 
				No need to specify anything here since we are not going to be adding this to the collsion grid
				*/
			}
			else if (std::string(focus_layer.name) == LayerIndicies::player_layer_name) {
				found_player_coordinates = true;
				discard_tile_system = true;
				m_collision_agent_ptr = &m_player;
				target_layer_world_cluster = LayerIndicies::humans_layer_index; // forging index
			}
			else if (std::string(focus_layer.name) == LayerIndicies::zombie_layer_name) {
				m_collision_agent_ptr = &m_zombie;
				discard_tile_system = true;
				target_layer_world_cluster = LayerIndicies::zombies_layer_index;
			}
			else if (std::string(focus_layer.name) == LayerIndicies::humans_layer_name) {
				m_collision_agent_ptr = &m_human;
				discard_tile_system = true;
				target_layer_world_cluster = LayerIndicies::humans_layer_index;
			}
			else {
				SBX_CRITICAL("Unidentified layer {} ",focus_layer.name);
			}

			if (discard_tile_system) {
				/*
					turn the tile system tiles into world clusters and add to the collsion grid
				*/

				SquareBox::GWOM::Layer & target_destination_layer = layers_[target_layer_world_cluster];

				if (target_destination_layer.world_clusters.size() == 0) {
					target_destination_layer.world_clusters.emplace_back();
					SquareBox::GWOM::WorldCluster & last_world_cluster = target_destination_layer.world_clusters.back();
					last_world_cluster.index = target_destination_layer.world_clusters.size() - 1;
				}

				const std::vector<std::pair<int, int>> & focus_layer_tiles = focus_layer.tile_system.active_tiles;
				for (unsigned int j = 0; j < focus_layer_tiles.size(); j++)
				{
					const std::pair<int, int> focus_layer_tile_coordinates = focus_layer_tiles[j];
					const SquareBox::GWOM::Tile * focus_tile_ptr = focus_layer.tile_system.getTile(focus_layer_tile_coordinates.first, focus_layer_tile_coordinates.second);
					if (focus_tile_ptr->key != -1) {
						/* where this layers newly crafted cluster objects should be kept */
						//the conversion
						//create a shell
						target_destination_layer.world_clusters.back().cluster_objects.emplace_back();
						//get a pointer to the shell
						SquareBox::GWOM::ClusterObject & new_cluster_object_ref = target_destination_layer.world_clusters.back().cluster_objects.back();
						new_cluster_object_ref.layer_index = target_destination_layer.index;
						new_cluster_object_ref.cluster_index = target_destination_layer.world_clusters.back().index;
						new_cluster_object_ref.index = target_destination_layer.world_clusters[new_cluster_object_ref.cluster_index].cluster_objects.size() - 1;

						// not overwritting these cluster objects componenets
						new_cluster_object_ref.radius = focus_layer.tile_system.getTileSize() *0.5f;
						new_cluster_object_ref.is_alive = true;
						new_cluster_object_ref.position = focus_tile_ptr->position;

						//the texture details
						std::pair<int, glm::vec4> texture_details = focus_layer.getTextureIdAndUvReactFromKey(focus_tile_ptr->key);
						new_cluster_object_ref.texture_info.texture_id = texture_details.first;
						new_cluster_object_ref.texture_info.uv_rect = texture_details.second;
						new_cluster_object_ref.shape = SquareBox::BodyShapeEnum::Circle;

						//setting some  plain old data to defaults
						new_cluster_object_ref.pod_int = -1;
						new_cluster_object_ref.pod_pair.first = -1;
						new_cluster_object_ref.pod_pair.second = -1;

						//add to the respective alive cluster objects 
						m_utilities.addPairToVector(layers_[new_cluster_object_ref.layer_index].alive_cluster_objects, std::pair<int, int>(new_cluster_object_ref.cluster_index, new_cluster_object_ref.index));
						//adding setting the allow dups specifier here would be a wast of resources
						//setting up the shape

						new_cluster_object_ref.shape = SquareBox::BodyShapeEnum::Circle;
						/* taking shape as to always be a circle because currently 
						 all memebers of our collision grid, (Zombie, Humans and Players are all circle shaped)
						*/

						//initialisation
						// add to the collision grid
						m_collision_agent_ptr->setUp(new_cluster_object_ref);
						m_collision_grid_ptr->addObject(new_cluster_object_ref);

						if (found_player_coordinates) {
							m_player_coordinates.first = new_cluster_object_ref.layer_index;
							m_player_coordinates.second.first = new_cluster_object_ref.cluster_index;
							m_player_coordinates.second.second = new_cluster_object_ref.index;
							player_coordinates_ = m_player_coordinates;
							game_play_camera_.setPosition(new_cluster_object_ref.position);
						}
					}
				}
			}
		}
		else {
			// the grass layer has one hug box of grass
			// for now lets forget about this
		}
	}

	// we need a layer that will hold the ammunition
	layers_.emplace_back(); 
	SquareBox::GWOM::Layer& ammunition_layer = layers_.back();
	m_utilities.nameLayerByIndex(ammunition_layer);
	ammunition_layer.index = layers_.size()-1;
	LayerIndicies::ammunition_layer_index = ammunition_layer.index;
	LayerIndicies::ammunition_layer_name = std::string(ammunition_layer.name);
	ammunition_layer.world_clusters.emplace_back();
	ammunition_layer.world_clusters.back().index = 0;

	ammunition_layer.particle_engine.init();
}
