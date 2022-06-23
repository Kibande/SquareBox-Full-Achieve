#include "Bullet.h"



void Bullet::init(SquareBox::AssetManager::GLTexture bullet_texture_, float bullet_size_, float speed_,float damage_, float bullet_decay_, int num_blood_mess_particles_, float blood_mess_particle_width_)
{
		m_bullet_texture = bullet_texture_;
		m_num_blood_mess_particles = num_blood_mess_particles_;
		m_blood_mess_particle_width = blood_mess_particle_width_;
		m_bullet_size = bullet_size_;
		m_speed = speed_;
		m_damage = damage_;
		m_bullet_decay = bullet_decay_;
		m_utilities.init();
}

void Bullet::setUp(SquareBox::GWOM::ClusterObject& cluster_object_)
{
	cluster_object_.speed = m_speed;
	cluster_object_.pod_float = m_damage; // the bullets damage is being stored as 
	cluster_object_.texture_info.texture_id = m_bullet_texture.id;
	cluster_object_.color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white).getIVec4();
	cluster_object_.radius = m_bullet_size;
	cluster_object_.shape = SquareBox::BodyShapeEnum::Circle;
}

void Bullet::update(float delta_time_, std::pair<int, std::pair<int, int>>& player_coordinates_, std::pair<int, std::pair<int, int>>& cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_, int grid_index_with_collision_members_, SquareBox::RenderEngine::ParticleBatch* blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
	SquareBox::GWOM::ClusterObject& target_bullet = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];
	glm::vec2 old_position = target_bullet.position;
	target_bullet.position += target_bullet.direction * target_bullet.speed * delta_time_;
	target_bullet.life_span -= m_bullet_decay;
	auto displacement = glm::length(target_bullet.position - old_position);
	if (target_bullet.life_span <= 0) {
		target_bullet.is_alive = false;
		collision_grid_ptr_->removeObject(target_bullet);
		m_utilities.removePairFromVector(layers_[target_bullet.layer_index].alive_cluster_objects, std::pair<int, int>(target_bullet.cluster_index, target_bullet.index));
		return;
	}

	//first collide with the world
	if (collideWithTiles(target_bullet,LayerIndicies::bricks_layer_index,layers_)) {
		// kill the bullet
		target_bullet.is_alive = false;
		collision_grid_ptr_->removeObject(target_bullet);
		m_utilities.removePairFromVector(layers_[target_bullet.layer_index].alive_cluster_objects, std::pair<int, int>(target_bullet.cluster_index, target_bullet.index));
		return;
	}
	// since the bullet is going to change the size of the cell members vector by
	// Either
	// dying
	// killing an agent
	std::vector<std::pair<int, std::pair<int, int>>> vector_of_cell_members = collision_grid_ptr_->getCellByIndex(grid_index_with_collision_members_)->member_cordinates;

	// collide with the none bullet members in the 
	for (unsigned int  i = 0; i < vector_of_cell_members.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& refreshed_bullet_cluster_object = layers_[cluster_object_coordinates_.first].world_clusters[cluster_object_coordinates_.second.first].cluster_objects[cluster_object_coordinates_.second.second];

		const std::pair<int, std::pair<int, int>>& cell_member_coordinates = vector_of_cell_members[i];
		SquareBox::GWOM::ClusterObject& cell_member_object = layers_[cell_member_coordinates.first].world_clusters[cell_member_coordinates.second.first].cluster_objects[cell_member_coordinates.second.second];

		if (cell_member_object.layer_index == LayerIndicies::humans_layer_index) {
			if (player_coordinates_ != cell_member_coordinates) {
				// bullets do not collide with the player
				if (collideAgent(refreshed_bullet_cluster_object, cell_member_object)) {
					// kill the bullet
					refreshed_bullet_cluster_object.is_alive = false;
					collision_grid_ptr_->removeObject(refreshed_bullet_cluster_object);
					m_utilities.removePairFromVector(layers_[refreshed_bullet_cluster_object.layer_index].alive_cluster_objects, std::pair<int, int>(refreshed_bullet_cluster_object.cluster_index, refreshed_bullet_cluster_object.index));
					return; // whether this return actualy helps us out 
				}
			}
		}
		else if (cell_member_object.layer_index == LayerIndicies::zombies_layer_index) {
			if (collideAgent(refreshed_bullet_cluster_object, cell_member_object)) {
				// apply the bullets damage to the zombie
				cell_member_object.life_span -= refreshed_bullet_cluster_object.pod_float;

				//spill some blood (pierce the zombies skin)
				createBloodMess(refreshed_bullet_cluster_object.position, blood_particle_batch_ptr_);

				// check if the zombie died
				if (cell_member_object.life_span < 0) {
					// kill the zombie
					cell_member_object.is_alive = false;
					collision_grid_ptr_->removeObject(cell_member_object);
					m_utilities.removePairFromVector(layers_[cell_member_object.layer_index].alive_cluster_objects, std::pair<int, int>(cell_member_object.cluster_index, cell_member_object.index));
				}

				// kill the bullet
				refreshed_bullet_cluster_object.is_alive = false;
				collision_grid_ptr_->removeObject(refreshed_bullet_cluster_object);
				m_utilities.removePairFromVector(layers_[refreshed_bullet_cluster_object.layer_index].alive_cluster_objects, std::pair<int, int>(refreshed_bullet_cluster_object.cluster_index, refreshed_bullet_cluster_object.index));
				return;
			}
		}
	}
}

bool Bullet::collideAgent(SquareBox::GWOM::ClusterObject& cluster_object_a_, SquareBox::GWOM::ClusterObject& cluster_object_b_)
{
	// Minimum distance before there is a collision
	const float MIN_DISTANCE = cluster_object_a_.radius + cluster_object_b_.radius;

	// Center position of this agent
	glm::vec2 centerPosA = cluster_object_a_.position;
	// Center position of the parameter agent
	glm::vec2 centerPosB = cluster_object_b_.position;

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0) {
		return true;
	}
	return false;
}

bool Bullet::collideWithTiles(SquareBox::GWOM::ClusterObject& cluster_object_, int target_tiled_layer_, std::vector<SquareBox::GWOM::Layer>& layers_)
{
	if (!layers_[target_tiled_layer_].tile_system.isInTileSystem(cluster_object_.position)) {
		return true;
	}
	// since bullets are smaller than the layers tile size
	return (layers_[target_tiled_layer_].tile_system.getTile(cluster_object_.position)->key!=-1);
}
void Bullet::dispose()
{
	m_utilities.dispose();
}

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Bullet::createBloodMess(const glm::vec2& position_, SquareBox::RenderEngine::ParticleBatch* blood_particle_batch_ptr_)
{
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 360.0f);
	glm::vec2 particle_velocity(0.01f, 0.0f);
	for (int i = 0; i < m_num_blood_mess_particles; i++) {
		blood_particle_batch_ptr_->addParticle(position_, glm::rotate(particle_velocity, randAngle(randEngine)), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::red), m_blood_mess_particle_width);
	}
}
