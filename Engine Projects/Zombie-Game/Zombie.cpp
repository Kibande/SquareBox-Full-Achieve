#include "Zombie.h"

void Zombie::init()
{

}

void Zombie::setUp(SquareBox::GWOM::ClusterObject & zombie_cluster_object)
{
	//give zombie a random initial direction

	std::mt19937 randomEngine = std::mt19937(time(nullptr));
	std::uniform_real_distribution<float> randDir = std::uniform_real_distribution<float>(-direction_range, direction_range);
	//Get radom direction
	//Make sure direction isn't zero
	if (zombie_cluster_object.direction.length() == 0) {
		zombie_cluster_object.direction = glm::vec2(1.0f, 0.0f);
	}
	zombie_cluster_object.direction = glm::normalize(zombie_cluster_object.direction);
	zombie_cluster_object.speed = m_speed;
	zombie_cluster_object.pod_pair = std::pair<int, int>(0, 0);
}

void Zombie::update(float delta_time_, std::pair<int, std::pair<int, int>> & cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_,int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
	std::vector<std::pair<int, std::pair<int, int>>> & vector_of_cell_members = collision_grid_ptr_->getCellByIndex(grid_index_with_collision_members_)->member_cordinates;

	bool got_target = false;
	float smallestDistance = 9999999.0f;
	SquareBox::GWOM::ClusterObject & zombie_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];
	
	// collide with the world
	collideWithTiles(delta_time_,zombie_cluster_object, LayerIndicies::bricks_layer_index, layers_);

	if (getClosetHuman(zombie_cluster_object,layers_,collision_grid_ptr_)) {
		//move the zombie towards its target
		zombie_cluster_object.position += zombie_cluster_object.direction * zombie_cluster_object.speed * delta_time_;
	}
	else {
		//move the zombie randomly like a human would

		std::mt19937 randomEngine(time(nullptr));
		std::uniform_real_distribution<float> randRotate(-rotation_angle_range, rotation_angle_range);

		zombie_cluster_object.position += zombie_cluster_object.direction * zombie_cluster_object.speed * delta_time_;

		// Randomly > 20 & < 250 random frames
		float random_number = SquareBox::MathLib::Random::Float() * 98682;
		if ((random_number > 20 && random_number < 250)) {
			zombie_cluster_object.direction = glm::rotate(zombie_cluster_object.direction, SquareBox::MathLib::Random::Float());
		}
	}

	for (auto i = start_index_; i < vector_of_cell_members.size(); i++)
	{
		SquareBox::GWOM::ClusterObject & refreshed_zombie_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];
		/* renew our cluster object refrence just incase a new zombie got added in the previous loop*/
		const std::pair<int, std::pair<int, int>> & cell_member_coordinates = vector_of_cell_members[i];
		SquareBox::GWOM::ClusterObject & cell_member_object = layers_[cell_member_coordinates.first].world_clusters[cell_member_coordinates.second.first].cluster_objects[cell_member_coordinates.second.second];

		if (cell_member_object.layer_index == LayerIndicies::humans_layer_index) {
			//Human
			if (collideAgent(delta_time_, refreshed_zombie_cluster_object, cell_member_object)) {
				turnHumanIntoZombie(cell_member_object, refreshed_zombie_cluster_object, layers_, collision_grid_ptr_);
			}
		}
		else if (cell_member_object.layer_index == LayerIndicies::zombies_layer_index) {
			if (collideAgent(delta_time_, refreshed_zombie_cluster_object, cell_member_object)) {
			}
		}
	}
}

void Zombie::dispose()
{
}

bool Zombie::getClosetHuman(SquareBox::GWOM::ClusterObject& zombie_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
	float smallestDistance = 9999999.0f;
	bool found_nearby_human = false;
	SquareBox::GWOM::Cell * current_zombie_cell = zombie_.associated_pointer;
	int current_cell_col=current_zombie_cell->coordinates.x;
	int current_cell_row = current_zombie_cell->coordinates.y;
	if (getClosetHumanInCell(zombie_, layers_, current_zombie_cell, smallestDistance)) {
		found_nearby_human = true;
	}
	

	if (current_cell_col > 0) {
		// left
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col - 1, current_cell_row);
		if(getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
			found_nearby_human = true;
		}
	}

	if (current_cell_col < collision_grid_ptr_->getNumXCells()) {
		// right
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col + 1, current_cell_row);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

//up down
	if (current_cell_row > 0) {
		// up
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col, current_cell_row - 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

	if (current_cell_row < collision_grid_ptr_->getNumYCells()) {
		// down
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col, current_cell_row + 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

// right up down

	if (current_cell_col < collision_grid_ptr_->getNumXCells() && current_cell_row < collision_grid_ptr_->getNumYCells()) {
		// right up
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col +1, current_cell_row + 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

	if (current_cell_col < collision_grid_ptr_->getNumXCells()  && current_cell_row > 0) {
		// right down
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col + 1, current_cell_row - 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

// left up down

	if (current_cell_col > 0  && current_cell_row < collision_grid_ptr_->getNumYCells()) {
		// left up
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col - 1, current_cell_row + 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

	if (current_cell_col > 0  && current_cell_row > 0) {
		// left down
		auto target_cell = collision_grid_ptr_->getCell(current_cell_col - 1, current_cell_row - 1);
			if (getClosetHumanInCell(zombie_, layers_, target_cell, smallestDistance)) {
				found_nearby_human = true;
			}
	}

	/// <summary>
	/// This is the zombies' ability to see
	/// in the future , after adding proper speed to them 
	/// we shall determine whether there is need to expand the zombies'
	/// vision
	/// </summary>
	/// <param name="zombie_"></param>
	/// <param name="layers_"></param>
	/// <param name="collision_grid_ptr_"></param>
	/// <returns></returns>

	return found_nearby_human;
}

bool Zombie::getClosetHumanInCell(SquareBox::GWOM::ClusterObject& zombie_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::Cell* target_cell_, float & current_closet_distance_)
{
	bool found_nearby_human = false;
	auto vector_of_cell_members = target_cell_->member_cordinates;
	for (unsigned int i = 0; i < vector_of_cell_members.size(); i++)
	{

		const std::pair<int, std::pair<int, int>>& cell_member_coordinates = vector_of_cell_members[i];
		SquareBox::GWOM::ClusterObject& cell_member_object = layers_[cell_member_coordinates.first].world_clusters[cell_member_coordinates.second.first].cluster_objects[cell_member_coordinates.second.second];
		if (cell_member_object.layer_index == LayerIndicies::humans_layer_index && cell_member_object.is_alive)
		{
			// Get the direction vector
			glm::vec2 distVec = cell_member_object.position - zombie_.position;
			// Get distance
			float distance = glm::length(distVec);

			// If this person is closer than the closest person, this is the new closest
			if (distance < current_closet_distance_) {
				current_closet_distance_ = distance;
				//target_coordinates = cell_member_coordinates;
				zombie_.direction = glm::normalize(distVec);
				zombie_.pod_int = cell_member_coordinates.first;
				zombie_.pod_pair = std::pair<int, int>(cell_member_coordinates.second.first, cell_member_coordinates.second.second);
				found_nearby_human = true;
			}
		}
	}

	return found_nearby_human;
}


