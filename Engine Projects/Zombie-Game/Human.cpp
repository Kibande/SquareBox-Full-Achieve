#include "Human.h"

void Human::init()
{
}

void Human::setUp(SquareBox::GWOM::ClusterObject & human_cluster_object)
{
	
	//Get random direction
	human_cluster_object.direction = glm::vec2(SquareBox::MathLib::Random::Float(), SquareBox::MathLib::Random::Float());
	//Make sure direction isn't zero
	if (human_cluster_object.direction.length() == 0) {
		human_cluster_object.direction = glm::vec2(1.0f, 0.0f);
	}
	human_cluster_object.direction = glm::normalize(human_cluster_object.direction);
	human_cluster_object.speed = m_speed;
}

void Human::update(float delta_time_, std::pair<int, std::pair<int, int>> & cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_,int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
	SquareBox::GWOM::ClusterObject & human_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];
	// not safe to have a refrenece here because the member_cordinates vector changes size when a human is turned 
	// to a zombie , but a zombie takes up there position so the size remains the same , so which makes it safe
	// unless we introduce something that kills humans but does not leave a zombie in its place !!
	// having a refrence here is something we should always keep in mind when making changes
	std::vector<std::pair<int, std::pair<int, int>>> & vector_of_cell_members = collision_grid_ptr_->getCellByIndex(grid_index_with_collision_members_)->member_cordinates;

	std::mt19937 randomEngine(time(nullptr));
	std::uniform_real_distribution<float> randRotate(-rotation_angle_range, rotation_angle_range);

	human_cluster_object.position += human_cluster_object.direction * human_cluster_object.speed * delta_time_;

	// Randomly > 20 & < 250 random frames
	float random_number = SquareBox::MathLib::Random::Float()*98682;
	if ((random_number>20 && random_number<250)) {
		human_cluster_object.direction = glm::rotate(human_cluster_object.direction, SquareBox::MathLib::Random::Float());
	}
	
	//collide with the tiles near us
	if (collideWithTiles(delta_time_, human_cluster_object, LayerIndicies::bricks_layer_index, layers_)) {
		human_cluster_object.direction = glm::rotate(human_cluster_object.direction, randRotate(randomEngine));
	}

	for (auto i = start_index_; i < vector_of_cell_members.size(); i++)
	{
		SquareBox::GWOM::ClusterObject & refreshed_human_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];

		const std::pair<int, std::pair<int, int>> & cell_member_coordinates = vector_of_cell_members[i];
		SquareBox::GWOM::ClusterObject & cell_member_object = layers_[cell_member_coordinates.first].world_clusters[cell_member_coordinates.second.first].cluster_objects[cell_member_coordinates.second.second];

		if (cell_member_object.layer_index == LayerIndicies::humans_layer_index) {
			//Human
			if (collideAgent(delta_time_, refreshed_human_cluster_object, cell_member_object)) {

			}
		}
		else if (cell_member_object.layer_index == LayerIndicies::zombies_layer_index) {
			//Zombie
			if (collideAgent(delta_time_, refreshed_human_cluster_object, cell_member_object)) {
			//turn human into zombie
				turnHumanIntoZombie(refreshed_human_cluster_object, cell_member_object, layers_, collision_grid_ptr_);
			}
		}
	}
}

void Human::dispose()
{
}
