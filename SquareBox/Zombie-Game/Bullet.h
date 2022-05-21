#pragma once
#include "ICollisionAgent.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "LayerIndicies.h"
#include <ctime>
#include <random>
#include <algorithm>
class Bullet
{
public:
	void init(SquareBox::AssetManager::GLTexture bullet_texture_,float bullet_size_,float speed_, float damage_,int num_blood_mess_particles_,float blood_mess_particle_width_);
	// should be called after the cluster objects position and direction have already been set
	void setUp(SquareBox::GWOM::ClusterObject& cluster_object_);
	void update(float delta_time_, std::pair<int, std::pair<int, int>>& player_coordinates_, std::pair<int, std::pair<int, int>>& cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_, int grid_index_with_collision_members_, SquareBox::RenderEngine::ParticleBatch* blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_);
	/* 
		other world objects do not collide with bulltes , rather.
		it is bullets that collide with them
	
	*/
	
	bool collideAgent(SquareBox::GWOM::ClusterObject& cluster_object_a_, SquareBox::GWOM::ClusterObject& cluster_object_b_);
	bool collideWithTiles(SquareBox::GWOM::ClusterObject& cluster_object_, int target_tiled_layer_, std::vector<SquareBox::GWOM::Layer>& layers_); void dispose();
	Bullet();
	~Bullet();

private:
	void createBloodMess(const glm::vec2& position_,SquareBox::RenderEngine::ParticleBatch* blood_particle_batch_ptr_);
	SquareBox::AssetManager::GLTexture m_bullet_texture;
	float m_speed;
	float m_damage;
	int m_num_blood_mess_particles;
	float m_blood_mess_particle_width;
	SquareBox::Utilities m_utilities;
	float m_bullet_size;
};

