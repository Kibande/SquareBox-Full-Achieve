#include "Player.h"

void Player::init()
{
}

void Player::setUp(SquareBox::GWOM::ClusterObject & player_cluster_object)
{
	player_cluster_object.speed = m_speed;
}

void Player::update(float delta_time_, std::pair<int, std::pair<int, int>> & cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_,int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
	SquareBox::GWOM::ClusterObject & player_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];
	std::vector<std::pair<int, std::pair<int, int>>> & vector_of_cell_members = collision_grid_ptr_->getCellByIndex(grid_index_with_collision_members_)->member_cordinates;
	
	//collide with the world next to us
	collideWithTiles(delta_time_,player_cluster_object, LayerIndicies::bricks_layer_index, layers_);

	for (auto i = start_index_; i < vector_of_cell_members.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& refreshed_player_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];

		const std::pair<int, std::pair<int, int>> & cell_member_coordinates = vector_of_cell_members[i];
		SquareBox::GWOM::ClusterObject & cell_member_object = layers_[cell_member_coordinates.first].world_clusters[cell_member_coordinates.second.first].cluster_objects[cell_member_coordinates.second.second];
		if (cell_member_object.layer_index == LayerIndicies::humans_layer_index) {
			//Human
			if (collideAgent(delta_time_, refreshed_player_cluster_object, cell_member_object)) {
			}
		}
		else if (cell_member_object.layer_index == LayerIndicies::zombies_layer_index) {
			//Zombie
			if (collideAgent(delta_time_, refreshed_player_cluster_object, cell_member_object)) {
			//	turn player into zombie
				turnHumanIntoZombie(refreshed_player_cluster_object, cell_member_object, layers_, collision_grid_ptr_);
			}
		}
	}
}

void Player::dispose()
{
}
